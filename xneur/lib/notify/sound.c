/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  Copyright (C) 2006-2016 XNeur Team
 *
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#ifdef WITH_SOUND

#ifdef WITH_GSTREAMER

#include <gst/gst.h>

#elif WITH_OPENAL

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#elif WITH_APLAY

#include <signal.h>
#include <stdio.h>

#endif

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "xnconfig.h"
#include "xnconfig_files.h"

#include "debug.h"
#include "log.h"

extern struct _xneur_config *xconfig;

#ifdef WITH_GSTREAMER

void sound_init(void)
{
	if (!xconfig->play_sounds)
		return;

	gst_init(NULL, NULL);
}

void sound_uninit(void)
{
	if (!xconfig->play_sounds)
		return;

	/*
	It is normally not needed to call this function in a normal application as
	the resources will automatically be freed when the program terminates.
	*/
	//gst_deinit();
}

static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data)
{
	if (bus){}

	GMainLoop *loop = (GMainLoop *) data;

	switch (GST_MESSAGE_TYPE(msg))
	{
		case GST_MESSAGE_EOS:
		{
		    //g_print ("End of stream\n");
			g_main_loop_quit(loop);
			break;
		}
		case GST_MESSAGE_ERROR:
		{
			gchar *debug;
			GError *err;

			gst_message_parse_error(msg, &err, &debug);
			g_free(debug);
			g_printerr ("Error: %s\n", err->message);
			g_error_free(err);

			g_main_loop_quit(loop);
			break;
		}
		default:
			break;
	}

	return TRUE;
}

static void on_pad_added(GstElement *element, GstPad *pad, gpointer data)
{
	if (element){}

	GstPad *sinkpad;
	GstElement *decoder = (GstElement *) data;

	// We can now link this pad with the vorbis-decoder sink pad 
	//g_print ("Dynamic pad created, linking demuxer/decoder\n");

	sinkpad = gst_element_get_static_pad (decoder, "sink");

	gst_pad_link (pad, sinkpad);

	gst_object_unref (sinkpad);
}

void *play_file_thread(void *param)
{
	char *path = (char *) param;
	log_message(TRACE, _("Play sound sample %s (use Gstreamer engine)"), path);


	// Initialize GStreamer
	GMainLoop *loop = g_main_loop_new(NULL, FALSE);

	GstElement *pipeline, *source, *demuxer, *decoder, *conv, *sink, *volume;
	GstBus *bus;
	guint bus_watch_id;
	/* Create gstreamer elements */
	pipeline = gst_pipeline_new ("audio-player");
	gst_element_set_state (pipeline, GST_STATE_NULL);

	source   = gst_element_factory_make ("filesrc", NULL);
	demuxer = gst_element_factory_make("decodebin", NULL);
	decoder = gst_element_factory_make("audioconvert", NULL);
	conv     = gst_element_factory_make ("audioconvert",  NULL);
	sink     = gst_element_factory_make ("autoaudiosink", NULL);
	volume = gst_element_factory_make("volume", NULL);

	if (!pipeline)
	{
		free(path);
		log_message(ERROR, _("Failed to create gstreamer context (pipeline)"));
		g_main_loop_unref(loop);
		return NULL;
  	}

	if (!source)
	{
		free(path);
		log_message(ERROR, _("Failed to create gstreamer context (source)"));
		g_main_loop_unref(loop);
		return NULL;
  	}
	
	if (!demuxer)
	{
		free(path);
		log_message(ERROR, _("Failed to create gstreamer context (demuxer)"));
		g_main_loop_unref(loop);
		return NULL;
  	}

	if (!decoder)
	{
		free(path);
		log_message(ERROR, _("Failed to create gstreamer context (decoder)"));
		g_main_loop_unref(loop);
		return NULL;
  	}
	
	if (!conv)
	{
		free(path);
		log_message(ERROR, _("Failed to create gstreamer context (conv)"));
		g_main_loop_unref(loop);
		return NULL;
  	}

	if (!volume)
	{
		free(path);
		log_message(ERROR, _("Failed to create gstreamer context (volume)"));
		g_main_loop_unref(loop);
		return NULL;
  	}

	if (!sink)
	{
		free(path);
		log_message(ERROR, _("Failed to create gstreamer context (sink)"));
		g_main_loop_unref(loop);
		return NULL;
  	}

	// Set volume
	double i = (double) xconfig->volume_percent / 100.0;
	g_object_set (G_OBJECT (volume), "volume", (double)i, NULL);

	/* we set the input filename to the source element */
	g_object_set (G_OBJECT (source), "location", path, NULL);

	g_signal_connect(demuxer, "pad-added", G_CALLBACK(on_pad_added), decoder);

	gst_bin_add_many (GST_BIN (pipeline), source, demuxer, decoder, volume, conv, sink, NULL);

	gst_element_link (source, demuxer);
	gst_element_link_many (decoder, volume, conv, sink, NULL);  
   
	/* we add a message handler */
	bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
	bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
	gst_object_unref (bus);


	/* Set the pipeline to "playing" state*/
	//g_print ("Now playing: %s\n", path);
	gst_element_set_state (pipeline, GST_STATE_PLAYING);

	/* Iterate */
	//g_print ("Running...\n");
	g_main_loop_run (loop);

	/* Out of the main loop, clean up nicely */
	//g_print ("Returned, stopping playback\n");
	gst_element_set_state (pipeline, GST_STATE_NULL);

	//g_print ("Deleting pipeline\n");
	gst_object_unref (GST_OBJECT (pipeline));
	g_source_remove (bus_watch_id);
	g_main_loop_unref (loop);
			
	free(path);
	
	return NULL;
}

#elif WITH_OPENAL /* WITH_OPENAL */

void sound_init(void)
{
	if (!xconfig->play_sounds)
		return;

	alutInit(NULL, NULL);
	alGetError();
	ALCcontext *pContext = alcGetCurrentContext();
	ALCdevice *pDevice = alcGetContextsDevice(pContext);
	log_message(TRACE, _("Initializing ALCdevice: %s "), alcGetString(pDevice, ALC_DEVICE_SPECIFIER));
}

void sound_uninit(void)
{
	if (!xconfig->play_sounds)
		return;

	alutExit();
}

void *play_file_thread(void *param)
{
	char *path = (char *) param;
	log_message(TRACE, _("Play sound sample %s (use OpenAL library)"), path);

	ALuint AlutBuffer = alutCreateBufferFromFile(path);
	if (!AlutBuffer)
	{
		free(path);
		log_message(ERROR, _("Failed to create OpenAL buffer"));
		return NULL;
	}

	ALuint AlutSource;
	alGenSources(1, &AlutSource);
	alSourcei(AlutSource, AL_BUFFER, AlutBuffer);
	double i = (double) xconfig->volume_percent / 100.0;
	alSourcef(AlutSource, AL_GAIN, (double)i);
	alSourcePlay(AlutSource);

	ALint result;
	alGetSourcei(AlutSource, AL_SOURCE_STATE, &result);
	if (result == AL_PLAYING)
	{
		sleep(1);
		alGetSourcei(AlutSource, AL_SOURCE_STATE, &result);
	}

	do
	{
		alDeleteSources(1, &AlutSource);
	}
	while (alGetError() != AL_NO_ERROR);

	do
	{
		alDeleteBuffers(1, &AlutBuffer);
	}
	while (alGetError() != AL_NO_ERROR);

	free(path);
	return NULL;
}

#endif
#ifdef WITH_APLAY /* WITH_APLAY */

void sound_init(void)
{
}

void sound_uninit(void)
{
}

void *play_file_thread(void *param)
{
	char *path = (char *) param;
	log_message(TRACE, _("Play sound sample %s (use aplay)"), path);

	static const char *program_name = "aplay";

	char *command = malloc((strlen(path) + strlen(program_name) + 1) * sizeof(char));
	sprintf(command, "%s %s", program_name, path);
	if (system(command) == -1)
		log_message(ERROR, _("Can't execute command '%s'"), command);

	free(command);

	free(path);
	return NULL;
}

#endif /* WITH_APLAY */

void play_file(int file_type)
{
	if (!xconfig->play_sounds)
		return;
	
	if (xconfig->sounds[file_type].file == NULL)
		return;
	
	if (!xconfig->sounds[file_type].enabled)
		return;
	
	char *path = get_file_path_name(SOUNDDIR, xconfig->sounds[file_type].file);
	if (path == NULL)
		return;
	
	pthread_attr_t sound_thread_attr;
	pthread_attr_init(&sound_thread_attr);
	pthread_attr_setdetachstate(&sound_thread_attr, PTHREAD_CREATE_DETACHED);

	pthread_t sound_thread;
	pthread_create(&sound_thread, &sound_thread_attr, &play_file_thread, (void *) path);

	pthread_attr_destroy(&sound_thread_attr);
}

#else /* WITH_SOUND */

void sound_init(void)
{
}

void sound_uninit(void)
{
}

void play_file(int file_type)
{
	if (file_type){}
}

#endif /* WITH_SOUND */

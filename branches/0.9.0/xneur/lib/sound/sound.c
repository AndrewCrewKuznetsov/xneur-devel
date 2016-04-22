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
 *  Copyright (C) 2006-2008 XNeur Team
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
#include <string.h>
#include <stdio.h>

#endif

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "xnconfig.h"
#include "xnconfig_files.h"

#include "debug.h"
#include "log.h"

extern struct _xneur_config *xconfig;

static pthread_mutex_t sound_mutex = PTHREAD_MUTEX_INITIALIZER;

#ifdef WITH_GSTREAMER

void sound_init(void)
{
	if (xconfig->sound_mode != SOUND_ENABLED)
		return;

	gst_init(NULL, NULL);
}

void sound_uninit(void)
{
	if (xconfig->sound_mode != SOUND_ENABLED)
		return;

	/*
	It is normally not needed to call this function in a normal application as 
	the resources will automatically be freed when the program terminates.
	*/
	//gst_deinit(); 
}

static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data)
{
	GMainLoop *loop = (GMainLoop *) data;

	switch (GST_MESSAGE_TYPE(msg))
	{
		case GST_MESSAGE_EOS:
		{
			g_main_loop_quit(loop);
			break;
		}
		case GST_MESSAGE_ERROR:
		{
			gchar *debug;
			GError *err;

			gst_message_parse_error(msg, &err, &debug);
			g_free(debug);
			g_error_free(err);

			g_main_loop_quit(loop);
			break;
		}
		default:
			break;
	}

	return TRUE;
}

static void new_pad(GstElement *element, GstPad *pad, gpointer data)
{
	GstElement *sink = data;	
	
	GstPad *alsapad = gst_element_get_pad(sink, "sink");
	gst_pad_link(pad, alsapad);
	gst_object_unref(alsapad);
}

void *play_file_thread(void *param)
{
	pthread_mutex_lock(&sound_mutex);

	// Initialize GStreamer
	GMainLoop *loop = g_main_loop_new(NULL, FALSE);
	
	// Initialize gst-elements
	GstElement *pipeline = gst_pipeline_new        ("audio-player");
	GstElement *source   = gst_element_factory_make("filesrc",  NULL);
	GstElement *parser   = gst_element_factory_make("wavparse", NULL);
	GstElement *sink     = gst_element_factory_make("alsasink", NULL);

	if (!pipeline || !source || !parser || !sink) 
	{
		log_message(ERROR, "Failed to create gstreamer context.");
		return NULL;
  	}

	char *path = (char *) param;
	log_message(TRACE, "Play sound sample %s (use Gstreamer engine)", path);

	gst_bin_add_many(GST_BIN(pipeline), source, parser, sink, NULL);
	gst_element_link(source, parser);

	g_signal_connect(parser, "pad-added", G_CALLBACK(new_pad), sink);

	// Set filename property on the file source. Also add a message handler. 
	g_object_set(G_OBJECT(source), "location", path, NULL);
	
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_bus_add_watch(bus, bus_call, loop);
	gst_object_unref(bus);

	gst_element_set_state(pipeline, GST_STATE_PLAYING);
	g_main_loop_run(loop);

	// Clean up nicely
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(GST_OBJECT(pipeline));
	
	free(path);
	pthread_mutex_unlock(&sound_mutex);
	return NULL;
}

#elif WITH_OPENAL /* WITH_GSTREAMER */

void sound_init(void)
{
	if (xconfig->sound_mode != SOUND_ENABLED)
		return;

	alutInit(NULL, NULL);
}

void sound_uninit(void)
{
	if (xconfig->sound_mode != SOUND_ENABLED)
		return;

	alutExit();
}

void *play_file_thread(void *param)
{
	pthread_mutex_lock(&sound_mutex);

	char *path = (char *) param;
	log_message(TRACE, "Play sound sample %s (use OpenAL library)", path);

	ALuint AlutBuffer = alutCreateBufferFromFile(path);
	if (!AlutBuffer)
	{
		free(path);
		pthread_mutex_unlock(&sound_mutex);
		return NULL;
	}

	ALuint AlutSource;
	alGenSources(1, &AlutSource);
	alSourcei(AlutSource, AL_BUFFER, AlutBuffer);
	alSourcePlay(AlutSource);

	do
		alDeleteSources(1, &AlutSource);
	while (alGetError() != AL_NO_ERROR);

	do
		alDeleteBuffers(1, &AlutBuffer);
	while (alGetError() != AL_NO_ERROR);

	free(path);
	pthread_mutex_unlock(&sound_mutex);
	return NULL;
}

#endif
#ifdef WITH_APLAY /* WITH_OPENAL */

void sound_init(void)
{
}

void sound_uninit(void)
{
}

void *play_file_thread(void *param)
{
	pthread_mutex_lock(&sound_mutex);

	char *path = (char *) param;
	log_message(TRACE, "Play sound sample %s (use ALSA library)", path);
	
	char *command = malloc((strlen(path) + 6) * sizeof(char));
	sprintf(command, "%s %s", "aplay", path);
	system(command);
	
	free(command);

	free(path);
	pthread_mutex_unlock(&sound_mutex);
	return NULL;
}

#endif /* WITH_APLAY */

void play_file(int file_type)
{
	if (xconfig->sound_mode != SOUND_ENABLED)
		return;

	pthread_attr_t sound_thread_attr;
	pthread_attr_init(&sound_thread_attr);
	pthread_attr_setdetachstate(&sound_thread_attr, PTHREAD_CREATE_DETACHED);

	char *path = get_file_path_name(SOUNDDIR, xconfig->sounds[file_type].file);

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

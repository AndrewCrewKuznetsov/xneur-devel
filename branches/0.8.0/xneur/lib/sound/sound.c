/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 *  (c) XNeur Team 2007
 *
 */
 
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef WITH_GSTREAMER

#include <unistd.h>
#include <gst/gst.h>

#elif WITH_OPENAL

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#endif

#include <stdlib.h>

#include "xnconfig.h"
#include "xnconfig_files.h"

#include "log.h"

extern struct _xneur_config *xconfig;

void sound_init(void)
{
	if (xconfig->sound_mode != SOUND_ENABLED)
		return;

#ifdef WITH_GSTREAMER
	gst_init(NULL, NULL);
#elif WITH_OPENAL
	alutInit(NULL, NULL);
#endif
}


void sound_uninit(void)
{
	if (xconfig->sound_mode != SOUND_ENABLED)
		return;

#ifdef WITH_GSTREAMER
	gst_deinit();
#elif WITH_OPENAL
	alutExit();
#endif
}

#ifdef WITH_GSTREAMER

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

			gst_message_parse_error (msg, &err, &debug);
			g_free(debug);

			log_message(ERROR, "Error: %s", err->message);
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
	
	GstPad *alsapad = gst_element_get_pad (sink, "sink");
	gst_pad_link(pad, alsapad);
	gst_object_unref(alsapad);
}

void play_file(int file_type)
{
	if (xconfig->sound_mode != SOUND_ENABLED)
		return;
	
	char *path = get_file_path_name(SOUNDDIR, xconfig->sounds[file_type]);
	
	// Initialize GStreamer
	GMainLoop *loop = g_main_loop_new (NULL, FALSE);
	
	// Initialize gst-elements
	GstElement *pipeline = gst_pipeline_new        ("audio-player");
	GstElement *source   = gst_element_factory_make("filesrc",  NULL);
	GstElement *parser   = gst_element_factory_make("wavparse", NULL);
	GstElement *sink     = gst_element_factory_make("alsasink", NULL);

	if (!pipeline || !source || !parser || !sink) 
	{
		log_message(ERROR, "Failed to create gstreamer context.");
		return;
  	}

	gst_bin_add_many(GST_BIN (pipeline), source, parser, sink, NULL);
	gst_element_link(source, parser);

	g_signal_connect(parser, "pad-added", G_CALLBACK (new_pad), sink);

	// Set filename property on the file source. Also add a message handler. 
	g_object_set(G_OBJECT (source), "location", path, NULL);
	
	GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE (pipeline));
	gst_bus_add_watch(bus, bus_call, loop);
	gst_object_unref(bus);

	gst_element_set_state (pipeline, GST_STATE_PLAYING);
	log_message(DEBUG, "Play sound sample %s (use Gstreamer engine)", path);
	g_main_loop_run(loop);

	// Clean up nicely
	free(path);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(GST_OBJECT (pipeline));
}

#elif WITH_OPENAL

void play_file(int file_type)
{
	if (xconfig->sound_mode != SOUND_ENABLED)
		return;
	
	char *path = get_file_path_name(SOUNDDIR, xconfig->sounds[file_type]);

	log_message(DEBUG, "Play sound sample %s (use OpenAL library)", path);

	ALuint AlutBuffer;
	AlutBuffer = alutCreateBufferFromFile(path);
	if (!AlutBuffer)
	{
		free(path);
		return;
	}

	free(path);

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
}

#else

void play_file(int file_type)
{
	file_type = file_type;
}

#endif

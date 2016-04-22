/* eggclock.c
 *
 * 
 * Authors:
 * Andrew Kuznetsov <andrewcrew@rambler.ru>
 *
 */

#include <gtk/gtk.h>
#include <math.h>
#include <time.h>

#include "eggclock.h"

G_DEFINE_TYPE (EggClock, egg_clock, GTK_TYPE_OBJECT);

gboolean egg_clock_update (gpointer data);

enum
{
	TIME_CHANGED,
	LAST_SIGNAL
};

guint egg_clock_signals[LAST_SIGNAL] = { 0 };

void clock_marshal_VOID__INT_INT_INT (GClosure *closure, GValue *return_value, guint n_param_values, const GValue *param_values, gpointer invocation_hint, gpointer marshal_data)
{
  if (return_value || invocation_hint) {};
  typedef void (*GMarshalFunc_VOID__INT_INT_INT) (gpointer data1, gint arg_1, gint arg_2, gint arg_3, gpointer data2);
  register GMarshalFunc_VOID__INT_INT_INT callback;
  register GCClosure *cc = (GCClosure*) closure;
  register gpointer data1, data2;

  g_return_if_fail (n_param_values == 4);

  if (G_CCLOSURE_SWAP_DATA (closure))
    {
      data1 = closure->data;
      data2 = g_value_peek_pointer (param_values + 0);
    }
  else
    {
      data1 = g_value_peek_pointer (param_values + 0);
      data2 = closure->data;
    }
  callback = (GMarshalFunc_VOID__INT_INT_INT) (marshal_data ? marshal_data : cc->callback);

  callback (data1,
            g_marshal_value_peek_int (param_values + 1),
            g_marshal_value_peek_int (param_values + 2),
            g_marshal_value_peek_int (param_values + 3),
            data2);
}

void egg_clock_class_init (EggClockClass *class)
{
	GObjectClass *obj_class;

	obj_class = G_OBJECT_CLASS (class);

	/* EggClock signals */
	egg_clock_signals[TIME_CHANGED] = g_signal_new (
			"time-changed",
			G_OBJECT_CLASS_TYPE (obj_class),
			G_SIGNAL_RUN_FIRST,
			G_STRUCT_OFFSET (EggClockClass, time_changed),
			NULL, NULL,
			clock_marshal_VOID__INT_INT_INT,
			G_TYPE_NONE, 3,
			G_TYPE_INT,
			G_TYPE_INT,
			G_TYPE_INT);
}


gboolean egg_clock_update (gpointer data)
{
	EggClock *clock;
	time_t timet;

	clock = EGG_CLOCK (data);
	
	/* update the time */
	time (&timet);
	
	// Send signal
	int hour = 0, minute = 0, second = 0;
	g_signal_emit (clock, egg_clock_signals[TIME_CHANGED], 0, hour, minute, second);
	
	return TRUE; /* keep running this event */
}

void egg_clock_init (EggClock *clock)
{
	egg_clock_update (clock);
	
	/* update the clock once a second */
	g_timeout_add (TIMER_PERIOD, egg_clock_update, clock);
}

GtkWidget *egg_clock_new(void)
{
	return g_object_new (EGG_TYPE_CLOCK, NULL);
}

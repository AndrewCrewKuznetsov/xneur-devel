/* eggclock.h
 *
 * 
 * Authors:
 * Andrew Kuznetsov <andrewcrew@rambler.ru>
 *
 */

#define TIMER_PERIOD 100

#ifdef G_ENABLE_DEBUG
#define g_marshal_value_peek_boolean(v)  g_value_get_boolean (v)
#define g_marshal_value_peek_char(v)     g_value_get_char (v)
#define g_marshal_value_peek_uchar(v)    g_value_get_uchar (v)
#define g_marshal_value_peek_int(v)      g_value_get_int (v)
#define g_marshal_value_peek_uint(v)     g_value_get_uint (v)
#define g_marshal_value_peek_long(v)     g_value_get_long (v)
#define g_marshal_value_peek_ulong(v)    g_value_get_ulong (v)
#define g_marshal_value_peek_int64(v)    g_value_get_int64 (v)
#define g_marshal_value_peek_uint64(v)   g_value_get_uint64 (v)
#define g_marshal_value_peek_enum(v)     g_value_get_enum (v)
#define g_marshal_value_peek_flags(v)    g_value_get_flags (v)
#define g_marshal_value_peek_float(v)    g_value_get_float (v)
#define g_marshal_value_peek_double(v)   g_value_get_double (v)
#define g_marshal_value_peek_string(v)   (char*) g_value_get_string (v)
#define g_marshal_value_peek_param(v)    g_value_get_param (v)
#define g_marshal_value_peek_boxed(v)    g_value_get_boxed (v)
#define g_marshal_value_peek_pointer(v)  g_value_get_pointer (v)
#define g_marshal_value_peek_object(v)   g_value_get_object (v)
#else /* !G_ENABLE_DEBUG */
/* WARNING: This code accesses GValues directly, which is UNSUPPORTED API.
 *          Do not access GValues directly in your code. Instead, use the
 *          g_value_get_*() functions
 */
#define g_marshal_value_peek_boolean(v)  (v)->data[0].v_int
#define g_marshal_value_peek_char(v)     (v)->data[0].v_int
#define g_marshal_value_peek_uchar(v)    (v)->data[0].v_uint
#define g_marshal_value_peek_int(v)      (v)->data[0].v_int
#define g_marshal_value_peek_uint(v)     (v)->data[0].v_uint
#define g_marshal_value_peek_long(v)     (v)->data[0].v_long
#define g_marshal_value_peek_ulong(v)    (v)->data[0].v_ulong
#define g_marshal_value_peek_int64(v)    (v)->data[0].v_int64
#define g_marshal_value_peek_uint64(v)   (v)->data[0].v_uint64
#define g_marshal_value_peek_enum(v)     (v)->data[0].v_long
#define g_marshal_value_peek_flags(v)    (v)->data[0].v_ulong
#define g_marshal_value_peek_float(v)    (v)->data[0].v_float
#define g_marshal_value_peek_double(v)   (v)->data[0].v_double
#define g_marshal_value_peek_string(v)   (v)->data[0].v_pointer
#define g_marshal_value_peek_param(v)    (v)->data[0].v_pointer
#define g_marshal_value_peek_boxed(v)    (v)->data[0].v_pointer
#define g_marshal_value_peek_pointer(v)  (v)->data[0].v_pointer
#define g_marshal_value_peek_object(v)   (v)->data[0].v_pointer
#endif /* !G_ENABLE_DEBUG */

#ifndef __EGG_CLOCK_FACE_H__
#define __EGG_CLOCK_FACE_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define EGG_TYPE_CLOCK		(egg_clock_get_type ())
#define EGG_CLOCK(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), EGG_TYPE_CLOCK, EggClock))
#define EGG_CLOCK_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), EGG_CLOCK, EggClockClass))
#define EGG_IS_CLOCK(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), EGG_TYPE_CLOCK))
#define EGG_IS_CLOCK_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), EFF_TYPE_CLOCK))
#define EGG_CLOCK_GET_CLASS	(G_TYPE_INSTANCE_GET_CLASS ((obj), EGG_TYPE_CLOCK, EggClockClass))

typedef struct _EggClock		EggClock;
typedef struct _EggClockClass	EggClockClass;

struct _EggClock
{
	GtkObject parent;

	int minute_offset; /* the offset of the minutes hand */	
};

struct _EggClockClass
{
	GtkObjectClass parent_class;

	void  (* time_changed)  (EggClock *clock,
          int hours, int minutes);
};

GtkWidget *egg_clock_new(void);
	
G_END_DECLS

#endif

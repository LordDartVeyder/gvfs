#include <config.h>

#include <glib/gi18n-lib.h>

#include "giotypes.h"
#include "gsimpleasyncresult.h"
#include "gloadableicon.h"

static void          g_loadable_icon_real_load_async  (GLoadableIcon        *icon,
						       int                   size,
						       GCancellable         *cancellable,
						       GAsyncReadyCallback   callback,
						       gpointer              user_data);
static GInputStream *g_loadable_icon_real_load_finish (GLoadableIcon        *icon,
						       GAsyncResult         *res,
						       char                **type,
						       GError              **error);
static void          g_loadable_icon_base_init        (gpointer              g_class);
static void          g_loadable_icon_class_init       (gpointer              g_class,
						       gpointer              class_data);

GType
g_loadable_icon_get_type (void)
{
  static GType loadable_icon_type = 0;

  if (! loadable_icon_type)
    {
      static const GTypeInfo loadable_icon_info =
	{
        sizeof (GLoadableIconIface), /* class_size */
	g_loadable_icon_base_init,   /* base_init */
	NULL,		/* base_finalize */
	g_loadable_icon_class_init,
	NULL,		/* class_finalize */
	NULL,		/* class_data */
	0,
	0,              /* n_preallocs */
	NULL
      };

      loadable_icon_type =
	g_type_register_static (G_TYPE_INTERFACE, I_("GLoadableIcon"),
				&loadable_icon_info, 0);

      g_type_interface_add_prerequisite (loadable_icon_type, G_TYPE_ICON);
    }

  return loadable_icon_type;
}

static void
g_loadable_icon_class_init (gpointer g_class,
			    gpointer class_data)
{
  GLoadableIconIface *iface = g_class;

  iface->load_async = g_loadable_icon_real_load_async;
  iface->load_finish = g_loadable_icon_real_load_finish;
}

static void
g_loadable_icon_base_init (gpointer g_class)
{
}

GInputStream *
g_loadable_icon_load (GLoadableIcon        *icon,
		      int                   size,
		      char                **type,
		      GCancellable         *cancellable,
		      GError              **error)
{
  GLoadableIconIface *iface;

  iface = G_LOADABLE_ICON_GET_IFACE (icon);

  return (* iface->load) (icon, size, type, cancellable, error);
  
}

void
g_loadable_icon_load_async (GLoadableIcon        *icon,
			    int                   size,
			    GCancellable         *cancellable,
			    GAsyncReadyCallback   callback,
			    gpointer              user_data)
{
  GLoadableIconIface *iface;

  iface = G_LOADABLE_ICON_GET_IFACE (icon);

  (* iface->load_async) (icon, size, cancellable, callback, user_data);
  
}

GInputStream *
g_loadable_icon_load_finish (GLoadableIcon        *icon,
			     GAsyncResult         *res,
			     char                **type,
			     GError              **error)
{
  GLoadableIconIface *iface;

  if (G_IS_SIMPLE_ASYNC_RESULT (res))
    {
      GSimpleAsyncResult *simple = G_SIMPLE_ASYNC_RESULT (res);
      if (g_simple_async_result_propagate_error (simple, error))
	return NULL;
    }
  
  iface = G_LOADABLE_ICON_GET_IFACE (icon);

  return (* iface->load_finish) (icon, res, type, error);
  
}

/********************************************
 *   Default implementation of async load   *
 ********************************************/

typedef struct {
  int size;
  char *type;
  GInputStream *stream;
} LoadData;

static void
load_data_free (LoadData *data)
{
  if (data->stream)
    g_object_unref (data->stream);
  g_free (data->type);
  g_free (data);
}

static void
load_async_thread (GSimpleAsyncResult *res,
		   GObject *object,
		   GCancellable *cancellable)
{
  GLoadableIconIface *iface;
  GInputStream *stream;
  LoadData *data;
  GError *error = NULL;
  char *type = NULL;

  data = g_simple_async_result_get_op_res_gpointer (res);
  
  iface = G_LOADABLE_ICON_GET_IFACE (object);
  stream = iface->load (G_LOADABLE_ICON (object), data->size, &type, cancellable, &error);

  if (stream == NULL)
    {
      g_simple_async_result_set_from_error (res, error);
      g_error_free (error);
    }
  else
    {
      data->stream = stream;
      data->type = type;
    }
}



static void
g_loadable_icon_real_load_async (GLoadableIcon        *icon,
				 int                   size,
				 GCancellable         *cancellable,
				 GAsyncReadyCallback   callback,
				 gpointer              user_data)
{
  GSimpleAsyncResult *res;
  LoadData *data;
  
  res = g_simple_async_result_new (G_OBJECT (icon), callback, user_data, g_loadable_icon_real_load_async);
  data = g_new0 (LoadData, 1);
  g_simple_async_result_set_op_res_gpointer (res, data, (GDestroyNotify) load_data_free);
  g_simple_async_result_run_in_thread (res, load_async_thread, 0, cancellable);
  g_object_unref (res);
}

static GInputStream *
g_loadable_icon_real_load_finish (GLoadableIcon        *icon,
				  GAsyncResult         *res,
				  char                **type,
				  GError              **error)
{
  GSimpleAsyncResult *simple = G_SIMPLE_ASYNC_RESULT (res);
  LoadData *data;

  g_assert (g_simple_async_result_get_source_tag (simple) == g_loadable_icon_real_load_async);

  data = g_simple_async_result_get_op_res_gpointer (simple);

  if (type)
    {
      *type = data->type;
      data->type = NULL;
    }

  return g_object_ref (data->stream);
}
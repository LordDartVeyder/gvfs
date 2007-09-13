#ifndef __G_VFS_DAEMON_DBUS_H__
#define __G_VFS_DAEMON_DBUS_H__

#include <glib.h>
#include <dbus/dbus.h>
#include <gvfs/gcancellable.h>

G_BEGIN_DECLS

typedef void (*GVfsAsyncDBusCallback) (DBusMessage *reply,
				       DBusConnection *conntection,
				       GError *io_error,
				       GCancellable *cancellable,
				       gpointer op_callback,
				       gpointer op_callback_data,
				       gpointer callback_data);
typedef void (*GetFdAsyncCallback)    (int fd,
				       gpointer callback_data);

int          _g_dbus_connection_get_fd_sync       (DBusConnection         *conn,
						   int                     fd_id);
gboolean     _g_dbus_message_iter_append_filename (DBusMessageIter        *iter,
						   const char             *filename);
void         _g_error_from_dbus                   (DBusError              *derror,
						   GError                **error);
void         _g_dbus_connection_get_fd_async      (DBusConnection         *connection,
						   int                     fd_id,
						   GetFdAsyncCallback      callback,
						   gpointer                callback_data);
void         _g_vfs_daemon_call_async             (const char             *owner,
						   DBusMessage            *message,
						   GMainContext           *context,
						   gpointer                op_callback,
						   gpointer                op_callback_data,
						   GVfsAsyncDBusCallback   callback,
						   gpointer                callback_data,
						   GCancellable           *cancellable);
DBusMessage *_g_vfs_daemon_call_sync              (const char             *owner,
						   DBusMessage            *message,
						   DBusConnection        **connection_out,
						   GCancellable           *cancellable,
						   GError                **error);
void         _g_dbus_connection_setup_with_main   (DBusConnection         *connection,
						   GMainContext           *context);

G_END_DECLS

#endif /* __G_VFS_DAEMON_DBUS_H__ */

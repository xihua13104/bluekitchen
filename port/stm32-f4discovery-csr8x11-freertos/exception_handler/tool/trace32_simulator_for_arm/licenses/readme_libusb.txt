For some scenarios, TRACE32 uses libusb licensed under version 2.1 of the GNU Lesser General Public License (LGPL):
https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html, a copy of this license is included with the TRACE32 software.

libusb is dynamically linked with TRACE32 using the included libusb-1.0.dll, libusb-1.0.so or libusb-1.0.dylib file, depending on the operating system.
These files are compiled based on the sources of https://github.com/libusb/libusb, commit 871eb299b989bc0714391804a9d2fdb145a32ff5.
The following changes have been made to these sources:

--- a/libusb/os/windows_winusb.c
+++ b/libusb/os/windows_winusb.c
@@ -1731,6 +1731,8 @@ static int windows_get_config_descriptor_by_value(struct libusb_device *dev, uin
 
 	for (index = 0; index < dev->num_configurations; index++) {
 		config_header = (PUSB_CONFIGURATION_DESCRIPTOR)priv->config_descriptor[index];
+		if (config_header == NULL)
+			return LIBUSB_ERROR_NOT_FOUND;
 		if (config_header->bConfigurationValue == bConfigurationValue) {
 			*buffer = priv->config_descriptor[index];
 			return (int)config_header->wTotalLength;

--- a/libusb/os/linux_usbfs.c
+++ b/libusb/os/linux_usbfs.c
@@ -27,6 +27,10 @@
 #include <ctype.h>
 #include <dirent.h>
 #include <errno.h>
+/* Older builds: O_CLOEXEC requires min. Linux 2.6.23 */
+#ifndef O_CLOEXEC
+#define O_CLOEXEC 0
+#endif
 #include <fcntl.h>
 #include <poll.h>
 #include <stdio.h>
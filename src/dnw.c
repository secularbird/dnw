/*
 * dnw.c -- Samsung USB bulk-transfer utility
 *
 * Copyright (C) 2011 Michel Stempin <michel.stempin@wanadoo.fr>
 * Based on previous work by Fox <hulifox008@163.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The names of the above-listed copyright holders may not be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation, either version 2 of that License or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#ifdef __LINUX__
#include <usb.h>
#elif defined __APPLE__
#include <libusb.h>
#endif
#include <sys/stat.h>
#include "config.h"
#include "gettext.h"

// Define shortcut for gettext()
#define _(string) gettext(string)

#define DEFAULT_ADDRESS      (0xC0008000)
#define	DEFAULT_BLOCK_SIZE   (512)
#define	DEFAULT_PRODUCT      (0x1234)
#define DEFAULT_VENDOR       (0x04E8)
#define DEFAULT_CONFIG       (1)
#define DEFAULT_INTERFACE    (0)
#define DEFAULT_EP           (0x02)
#define DEFAULT_TIMEOUT      (3000)

//==============================================================================
// Private Data
//==============================================================================

// Download address
static u_int32_t address = DEFAULT_ADDRESS;

// Transfer block size
static int block_size = DEFAULT_BLOCK_SIZE;

// USB Product ID
static unsigned int product = DEFAULT_PRODUCT;

// USB Vendor ID
static unsigned int vendor = DEFAULT_VENDOR;

// USB configuration number
static int configuration = DEFAULT_CONFIG;

// USB interface number
static int interface = DEFAULT_INTERFACE;

// USB bulk transfer endpoint
static int endpoint = DEFAULT_EP;

// USB transfer timeout
static int timeout = DEFAULT_TIMEOUT;

// Quiet mode
static int quiet = 0;

// Source filename
static char *filename = NULL;

//==============================================================================
// Private functions
//==============================================================================

// Print usage information
static void
print_usage(void)
{
  printf(_("Usage: dnw [options] file\n"));
  printf(_("Options:\n"));
  printf(_(" -a, -A, --address=ADDRESS              Specify the download ADDRESS (default is 0xc0008000)\n"));
  printf(_(" -b, -B, --block-size=SIZE              Specify the USB transfer block SIZE (default is 512)\n"));
  printf(_(" -c, -C, --config=VALUE                 Specify the USB configuration VALUE (default is 1)\n"));
  printf(_(" -d, -D, --device=VENDOR:PRODUCT        Specify the USB VENDOR:PRODUCT device ID (default is \"04E8:1234\")\n"));
  printf(_(" -e, -E, --endpoint=ADDRESS             Specify the USB transfer endpoint ADDRESS (default is 2)\n"));
  printf(_(" -h, -H, --help                         Print the option help\n"));
  printf(_(" -i, -I, --interface=NUMBER             Specify the USB interface number (default is 0)\n"));
  printf(_(" -q, -Q, --quiet                        Turn off display messages\n"));
  printf(_(" -t, -T, --timeout=DURATION             Specify the USB transfer timemout DURATION in ms (default is 3000)\n"));
  printf(_(" -v, -V, --version                      Print the version information\n"));
}

// Print version information
static void
print_version(void)
{
  printf("dnw version "VERSION"\n");
  printf(_("USB bulk-transfer utility for Samsung devices.\n"));
  printf(_("Copyright (C) 2011, Michel Stempin <michel.stempin@wanadoo.fr>\n"));
  printf(_("License GPL2+: GNU GPL version 2 or later <http://www.gnu.org/licenses/gpl-2.0.html>.\n"));
  printf(_("This is free software: you are free to change and redistribute it.\n"));
  printf(_("There is NO WARANTY, to the extent permitted by law.\n"));
}

// Parse command line options
static void
parse_options(int argc, char *argv[])
{
  struct option long_options[] = {
    {"address", 1, NULL, 0},
    {"block-size", 1, NULL, 0},
    {"config", 1, NULL, 0},
    {"device", 1, NULL, 0},
    {"endpoint", 1, NULL, 0},
    {"help", 0, NULL, 0},
    {"interface", 1, NULL, 0},
    {"quiet", 0, NULL, 0},
    {"timeout", 1, NULL, 0},
    {"version", 0, NULL, 0},
    {0, 0, NULL, 0}
  };
  int c, opt;

  while (1) {
    c = getopt_long(argc, argv, "a:A:b:B:e:E:hHi:i:qQt:T:vV", long_options, &opt);
    if (c == -1) {
      break;
    }
    if (c == 0) {
      if (!strcmp(long_options[opt].name, "address")) {
        c = 'a';
      } else if (!strcmp(long_options[opt].name, "block-size")) {
        c = 'b';
      } else if (!strcmp(long_options[opt].name, "config")) {
        c = 'c';
      } else if (!strcmp(long_options[opt].name, "device")) {
        c = 'd';
      } else if (!strcmp(long_options[opt].name, "endpoint")) {
        c = 'e';
      } else if (!strcmp(long_options[opt].name, "help")) {
        c = 'h';
      } else if (!strcmp(long_options[opt].name, "interface")) {
        c = 'i';
      } else if (!strcmp(long_options[opt].name, "quiet")) {
        c = 'q';
      } else if (!strcmp(long_options[opt].name, "timeout")) {
        c = 't';
      } else if (!strcmp(long_options[opt].name, "version")) {
        c = 'v';
      }
    }
    switch (c) {
    case 'a':
    case 'A':
      if (optarg != NULL) {
        address = atoi(optarg);
      }
      break;
    case 'b':
    case 'B':
      if (optarg != NULL) {
        block_size = atoi(optarg);
      }
      break;
    case 'c':
    case 'C':
      if (optarg != NULL) {
        configuration = atoi(optarg);
      }
      break;
    case 'd':
    case 'D':
      if (optarg != NULL) {
        sscanf(optarg, "%x:%x", &vendor, &product);
      }
      break;
    case 'e':
    case 'E':
      if (optarg != NULL) {
        endpoint = atoi(optarg);
      }
      break;
    case 'h':
    case 'H':
      print_usage();
      exit(EXIT_SUCCESS);
    case 'i':
    case 'I':
      if (optarg != NULL) {
        interface = atoi(optarg);
      }
      break;
    case 'q':
    case 'Q':
      if (optarg != NULL) {
        quiet = atoi(optarg);
      }
      break;
    case 't':
    case 'T':
      if (optarg != NULL) {
        timeout = atoi(optarg);
      }
      break;
    case 'v':
    case 'V':
      print_version();
      exit(EXIT_SUCCESS);
    default:
      print_usage();
      exit(EXIT_FAILURE);
    }
  }
  if (optind >= argc) {
    print_usage();
    exit(EXIT_FAILURE);
  }
  filename = strdup(argv[optind]);
}

// Get a handle on the target USB device
#ifdef __LINUX__
static struct usb_dev_handle *
#elif defined __APPLE__
static struct libusb_device_handle *
#endif
open_device(uint16_t vendor, uint16_t product, int configuration, int interface)
{
#ifdef __LINUX__
  struct usb_bus *busses, *bus;
  struct usb_device *device;
  struct usb_dev_handle *hd;

  usb_init();
  usb_find_busses();
  usb_find_devices();

  // Scan USB busses
  busses = usb_get_busses();
  for (bus = busses; bus; bus = bus->next) {

    // Scan USB devices
    for (device = bus->devices; device; device = device->next) {

      // Look for our device
      if (device->descriptor.idVendor == vendor &&
          device->descriptor.idProduct == product) {

        // Device found
        if (!quiet) {
          printf(_("Target USB device found!\n"));
        }
        hd = usb_open(device);
        if (!hd) {
          perror(_("Cannot open USB device"));
          return NULL;
        }
        if (usb_set_configuration(hd, configuration) < 0) {
          perror(_("Cannot set configuration for USB device"));
          usb_close(hd);
          return NULL;
        }
        if (usb_claim_interface(hd, interface) < 0) {
          perror(_("Cannot claim interface for USB device"));
          usb_close(hd);
          return NULL;
        }
        return hd;
      }
    }
  }
#elif defined __APPLE__
  struct libusb_context *ctx = NULL;
  struct libusb_device_handle *hd = NULL;

  int ret = 0;
  ret = libusb_init(&ctx);
  if(ret != 0) {
    printf("init libusb failed\n");
    return NULL;
  }

  hd = libusb_open_device_with_vid_pid(ctx, vendor, product);

  if (hd == NULL) {
    perror("failed to open the USB device");
    return NULL;
  }

  int config = -1;
  ret = libusb_get_configuration(hd, &config);
  if (ret < 0) {
    printf("set configure failed: %s\n", libusb_error_name(ret));
    libusb_close(hd);
    return NULL;
  }
  printf("default configuration is %d\n", config);
#if 0
  ret = libusb_reset_device(hd);;
  if (ret < 0) {
    printf("device reset failed: %s\n", libusb_error_name(ret));
    libusb_close(hd);
    return NULL;
  }
#endif
  libusb_device *usbdevice = libusb_get_device(hd);
  if(usbdevice == NULL) {
      printf("unbale to get usbdevice\n");
      libusb_close(hd);
      return NULL;
    }

  int busnumber = libusb_get_bus_number(usbdevice);
  printf("bus number is %u\n", busnumber);

  int address = libusb_get_device_address(usbdevice);
  printf("usb device address is %x\n", address);

  ret = libusb_set_configuration(hd, configuration);
  if (ret != 0) {
    printf("set configure failed: %s\n", libusb_error_name(ret));
    libusb_close(hd);
    return NULL;
  }
  ret = libusb_claim_interface(hd, interface);
  if (ret != 0) {
    printf("claim the interfaceA failed: %s\n", libusb_error_name(ret));
    libusb_close(hd);
    return NULL;
  }
  return hd;

#endif
  fprintf(stderr, _("Target USB device not found!\n"));
  return NULL;
}

// Read the source file to transfer
unsigned char *read_file(char *filename, unsigned int *length)
{
  unsigned char *buffer = NULL;
  struct stat fs;
  int fd = -1, i;
  u_int16_t checksum = 0;

  // Open the file to transfer
  fd = open(filename, O_RDONLY);
  if (fd == -1) {
    perror(_("Cannot open file"));
    return NULL;
  }

  // Get the file size
  if (fstat(fd, &fs) == -1) {
    perror(_("Cannot get file size"));
    goto error;
  }

  // Allocate buffer
  buffer = (unsigned char *) malloc(fs.st_size + 10);
  if (buffer == NULL) {
    perror(_("Cannot allocate buffer memory"));
    goto error;
  }

  // Read the whole file into memory
  if (read(fd, buffer + 8, fs.st_size) != fs.st_size) {
    perror(_("Cannot read file"));
    goto error;
  }

  // Compute checksum
  for (i = 8; i < fs.st_size + 8; i++) {
    checksum += buffer[i];
  }
  if (!quiet) {
    printf(_("Filename : %s\n"), filename);
    printf(_("Filesize : %ld bytes\n"), fs.st_size);
    printf(_("Checksum : %#04x\n"), checksum);
  }

  // Transfer header: download address & size
  *((u_int32_t *) buffer) = address;
  *((u_int32_t *) buffer + 1) = fs.st_size + 10;

  // Transfer footer: checksum
  *((u_int16_t *) (buffer + fs.st_size + 8)) = checksum;

  *length = fs.st_size + 10;
  return buffer;

error:
  if (fd != -1) {
    close(fd);
  }
  if (buffer != NULL) {
    free(buffer);
  }
  return NULL;

}

//==============================================================================
// Public functions
//==============================================================================

// Main function
int
main(int argc, char *argv[])
{
#ifdef __LINXU__
  struct usb_dev_handle *device = NULL;
#elif __APPLE__
  struct libusb_device_handle *device = NULL;
#endif
  unsigned int length = 0;
  unsigned char *buffer = NULL;
  unsigned int remain = 0;
  unsigned int to_write = 0;

  // Set up i18n
  setlocale(LC_ALL, "");
  textdomain("dnw");
  bindtextdomain("dnw", LOCALEDIR);

  // Parse command line options
  parse_options(argc, argv);

  // Open the USB device
  device = open_device(vendor, product, configuration, interface);
  if (!device) {
    goto error;
  }

  // Read the file into memory
  buffer = read_file(filename, &length);
  if (buffer == NULL) {
    goto error;
  }

  // Actually transfer the data
  remain = length;
  if (!quiet) {
    printf(_("Writing data...\n"));
  }
  while (remain) {
    to_write = (remain > block_size) ? block_size : remain;
#ifdef __LINUX__
    if (usb_bulk_write(device,
                       endpoint,
                       (char *) buffer + (length - remain),
                       to_write,
                       timeout) != to_write) {
      perror(_("USB transfer failed"));
      goto error;
    }
#elif defined __APPLE__
    int transfered = -1;
    if (libusb_bulk_transfer(device,
                       endpoint,
                       (char *) buffer + (length - remain),
                       to_write,
                       &transfered,
                             timeout)&&(transfered != to_write)) {
      perror(_("USB transfer failed"));
      goto error;
    }
#endif
    remain -= to_write;
    if (!quiet) {
      printf(_("\r%d%%\t %d bytes     "),
             (length - remain) * 100 / length,
             length - remain);
      fflush(stdout);
    }
  }
  if (!remain && !quiet) {
    printf(_("Done!\n"));
  }

  // Cleanup
  if (filename) {
    free(filename);
  }
  if (buffer) {
    free(buffer);
  }
  if (device) {
#ifdef __LINUX__
    usb_release_interface(device, interface);
    usb_close(device);
#elif defined __APPLE__
    libusb_release_interface(device, interface);
    libusb_close(device);
#endif
  }
  exit(EXIT_SUCCESS);

 error:
  if (filename) {
    free(filename);
  }
  if (buffer) {
    free(buffer);
  }
  if (device) {
#ifdef __LINUX__
    usb_release_interface(device, interface);
    usb_close(device);
#elif defined __APPLE__
    libusb_release_interface(device, interface);
    libusb_close(device);
#endif
  }
  exit(EXIT_FAILURE);
}

LOCAL_PATH := $(my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := src/dnw.c

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_MODULE := dnw

LOCAL_CFLAGS := -m64 -DLOCALEDIR=\"$(PWD)\"

LOCAL_LDFLAGS += -m64

LOCAL_LDFLAGS += -L/opt/local/lib -lusb-1.0

ifeq ($(HOST_OS),darwin)
	LOCAL_C_INCLUDES += /opt/local/include/libusb-1.0
endif

include $(BUILD_HOST_EXECUTABLE)

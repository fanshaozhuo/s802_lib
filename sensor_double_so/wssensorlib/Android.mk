LOCAL_PATH:= $(call my-dir)
#ָ��LOCAL_PATH���ڲ���Դ�ļ�


include $(CLEAR_VARS)
#Android.mk�п��Զ���������ģ�飬ÿ������ģ�鶼����include $(CLEAR_VARS)��ʼ
LOCAL_SRC_FILES:= \
	ws_sensor.c
#LOCAL_SRC_FILES�м���Դ�ļ�·��

LOCAL_CFLAGS:= -DLOG_TAG=\"WSSensorlib\"

LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libhardware \
	libutils \
	libbinder \
	libui \
	libgui \
	libc \
	libm \
	libdl 

#LOCAL_SHARED_LIBRARIES�м�������Ҫ���ӵĶ�̬�⣨*.so��������


LOCAL_MODULE:= libws_sensor
#LOCAL_MODULE��ʾģ�����յ�����

include $(BUILD_SHARED_LIBRARY)

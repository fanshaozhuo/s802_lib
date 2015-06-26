#include <stdint.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <stdarg.h>
#include <string.h>
#include <semaphore.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <hardware/sensors.h>
#include <utils/Timers.h>
#include <linux/input.h>

#define SENSOR_MAX_VALUE     20

#define EPOLLIN          0x00000001
#define EPOLLPRI         0x00000002
#define EPOLLOUT         0x00000004
#define EPOLLERR         0x00000008
#define EPOLLHUP         0x00000010
#define EPOLLRDNORM      0x00000040
#define EPOLLRDBAND      0x00000080
#define EPOLLWRNORM      0x00000100
#define EPOLLWRBAND      0x00000200
#define EPOLLMSG         0x00000400
#define EPOLLET          0x80000000
#define EPOLL_CTL_ADD    1
#define EPOLL_CTL_DEL    2
#define EPOLL_CTL_MOD    3

#define  __ANDROID
#define DEBUG(x)  ws_dprintf x
#define ssize_t          unsigned int

#ifdef __ANDROID
#include <android/log.h>
#define TAG_DEBUG "WSLIB"
#endif

static int32_t mMouseDeviceId = -1;
//static int32_t mJoystickDeviceId = -1;
static const uint32_t EPOLL_ID_SOKCET = 0x80000003;
static int mSocketFd = -1;
static int bAcc = 0;
static int bGyro = 0;
int mSensorCount;
int mVirtualSensor;       
int mMaxHalSensorHandle;

const int axisTab[][2] = {
	ABS_X, 0,
	ABS_Y, 1,
	ABS_Z, 11,
	ABS_RZ, 14,
	ABS_HAT0X, 15,
	ABS_HAT0Y, 16
};

typedef struct sensor_t_hal {
    /* name of this sensors */
    const char*     name;
    /* vendor of the hardware part */
    const char*     vendor;
    /* version of the hardware part + driver. The value of this field
     * must increase when the driver is updated in a way that changes the
     * output of this sensor. This is important for fused sensors when the
     * fusion algorithm is updated.
     */    
    int             version;
    /* handle that identifies this sensors. This handle is used to activate
     * and deactivate this sensor. The value of the handle must be 8 bits
     * in this version of the API. 
     */
    int             handle;
    /* this sensor's type. */
    int             type;
    /* maximaum range of this sensor's value in SI units */
    float           maxRange;
    /* smallest difference between two values reported by this sensor */
    float           resolution;
    /* rough estimate of this sensor's power consumption in mA */
    float           power;
    /* minimum delay allowed between events in microseconds. A value of zero
     * means that this sensor doesn't report events at a constant rate, but
     * rather only when a new data is available */
    int32_t         minDelay;
    /* reserved fields, must be zero */
    void*           reserved[8];
}sensor_t;

typedef struct sensors_module_t_hal {
    struct hw_module_t common;

    /**
     * Enumerate all available sensors. The list is returned in "list".
     * @return number of sensors in the list
     */
    int (*get_sensors_list)(struct sensors_module_t* module,
            struct sensor_t const** list);
}sensors_module_t;

typedef struct RawEvent_ws {
    nsecs_t when;
    int32_t deviceId;
    int32_t type;
    int32_t code;
    int32_t value;
}RawEvent;

/* Describes an absolute axis. */
typedef struct RawAbsoluteAxisInfo_ws {
    int valid; // true if the information is valid, false otherwise
    int32_t minValue;  // minimum value
    int32_t maxValue;  // maximum value
    int32_t flat;      // center flat position, eg. flat == 8 means center is between -8 and 8
    int32_t fuzz;      // error tolerance, eg. fuzz == 4 means value is +/- 4 due to noise
    int32_t resolution; // resolution in units per mm or radians per mm
}RawAbsoluteAxisInfo;

typedef union epoll_data {
    void *ptr;
    int fd;
    unsigned int u32;
    unsigned long long u64;
} epoll_data_t;

typedef struct epoll_event_ws {
    unsigned int events;
    epoll_data_t data;
}epoll_event;

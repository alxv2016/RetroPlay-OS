#ifndef _KEYMON_H
#define _KEYMON_H

// Button_flags
#define SELECT_BIT 0
#define START_BIT 1
#define SELECT (1 << SELECT_BIT)
#define START (1 << START_BIT)

#define SARADC_IOC_MAGIC 'a'
#define IOCTL_SAR_INIT _IO (SARADC_IOC_MAGIC, 0)
#define IOCTL_SAR_SET_CHANNEL_READ_VALUE _IO (SARADC_IOC_MAGIC, 1)

enum InputState { RELEASED, PRESSED, REPEAT };

typedef struct {
  int channel_value;
  int adc_value;
} SAR_ADC_CONFIG_READ;

struct input_event ev;

void quit(int exitcode);
void checkAXP(void);

#endif
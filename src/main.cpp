#include <arduino.h>
#include <gproc.h>
#include <gtick.h>
#include <gtick_stm32_hal.h>
#include <glog.h>
#include <gadc.h>
#include <gtty.h>
#include <stdio.h>

const int ledPin = PC13;
void loop_led() {
  static int sw=0,cnt=0;

  digitalWrite(ledPin, sw);

  printf("[%d]\n",cnt++);
  sw = !sw;
}

void test1() {
  //Serial.print(".");
  gdebug(3,".");
}

void test2() {
  static int c = 0;
  gdebug(2,"test2 cnt=%d test(2)\n",c++);
}

stm32adc adc;
void test3() {
  int val[4];

  for(int i=0;i<4;i++) {
    val[i] = adc.read();
  }

  gdebug(2,"adc = %ld %ld %ld %ld\n", val[0], val[1], val[2], val[3]);
}

void test4() {
  int val = Serial.read();
  if( val == -1 ) return;
  Serial.write(val); // echo
}

#include <gstr.h>
extern void view_proc_all();
void ls(const char *s) {
  if(s && *s) {
    int val = stol(s);
    view_proc(val);
  } else {
    view_proc_all();
  }
}

void setup() {
  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  Serial.println("Hello");
  
  log_level = 2;

  adc_channels ac[] = {
    { ADC_CHANNEL_2, ADC_SAMPLETIME_56CYCLES,GPIOA, 2,},
    { ADC_CHANNEL_9, ADC_SAMPLETIME_56CYCLES,GPIOB, 1, },
    { ADC_CHANNEL_17, ADC_SAMPLETIME_480CYCLES,0, 0, },
    { ADC_CHANNEL_18, ADC_SAMPLETIME_480CYCLES,0, 0, },
    0
  };

  adc.setup(ADC1,4,ac);

  init_ticks(eTICK_VOL_100us);
  init_pfn();

  set_tty_func("ls",ls );

  add_pfn(1000, loop_led, "test");
  add_pfn(100,test1,"N1");
  add_pfn(300,test2);
  //add_pfn(10000,view_proc_all);
  add_pfn(1200, test3,"adc read");
  add_pfn(0,tty,"key in");

  //timer.setPrescaleFactor(1);
  //timer.setCaptureCompare(1);
  //timer.attachInterrupt(timer_func);
  //timer.resume();

  scadule();
}

void loop() {  }
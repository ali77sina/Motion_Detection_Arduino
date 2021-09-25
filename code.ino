#include<TensorFlowLite.h>
#include "tfliteModel.h"      //the model's h-file
#include <Arduino_LSM9DS1.h>
#define RED 22
#define GREEN 23
#define BLUE 24


//function to calculate the average of the elements
//with length "num"
float average(float *x,int num){
  float sum = 0;
  int i = 0;
  for (i=0;i<num;i++){
    sum = sum + x[i];
  }
  return sum/10;
}



// Importing libraries 
#include "TensorFlowLite.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"    //includes micro operations in the model, not used in this script!
#include "tensorflow/lite/micro/micro_error_reporter.h"   //error log used with the interpreter 
#include "tensorflow/lite/micro/micro_interpreter.h"    //the interpreter for the tflit model
#include "tensorflow/lite/micro/all_ops_resolver.h"   //includes all of the operations currently supported by the tflite library 
#include "tensorflow/lite/version.h"


// TFLite globals, used for compatibility with Arduino-style sketches
namespace {
  tflite::ErrorReporter* error_reporter = nullptr;
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* model_input = nullptr;
  TfLiteTensor* model_output = nullptr;

  // Create an area of memory to use for input, output, and other TensorFlow
  // arrays. You'll need to adjust this by combiling, running, and looking
  // for errors.
  constexpr int kTensorArenaSize = 5 * 1024;    //5Kb
  uint8_t tensor_arena[kTensorArenaSize];
} // namespace

void setup() {

//Setting up the RGB LED
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

//Give error if the IMU fails to initate
  if (!IMU.begin()) {
  Serial.println("Failed to initialize IMU!");
  while (1);
}

  // Set up logging (will report to Serial, even within TFLite functions)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure
  model = tflite::GetModel(tfliteModel);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    error_reporter->Report("Model version does not match Schema");
    while(1);
  }

  // Pulling in all the operations tflite currently supports
  // it is more efficient to make an object of micro_mutable_op_resolver and pull only required operators based on your model
  static tflite::AllOpsResolver resolver;
  

  // Build an interpreter to run the model
  static tflite::MicroInterpreter static_interpreter(
    model, resolver, tensor_arena, kTensorArenaSize,
    error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    error_reporter->Report("AllocateTensors() failed");
    while(1);
  }

  // Assign model input and output buffers (tensors) to pointers
  model_input = interpreter->input(0);
  model_output = interpreter->output(0);
}

void loop() {   //main loop start
float x, y, z;    //variables to store the accelerometer data
int i = 0;        //dummy variable for loops and ...
float predictions[10];    //array to store 10 consecutive predictions
unsigned long start = millis();   //saving the starting time
unsigned long t;    //variable to store the time elapsed
float avg_pred = 0;   //variable to store the average value of the prediction


//turning off all the LEDs
//note the cathode is connected to the pins in Arduino nano, so the leds turn on when the pin is pulled down. 
digitalWrite(BLUE,HIGH);
digitalWrite(GREEN,HIGH);
digitalWrite(22,HIGH);

//wait for user to get ready
delay(1000);
//Red LED turns on, indicating it's time to do the motion
digitalWrite(22,LOW);

//calculating 10 predictions
for(int j=0;j<10;j++){  //predictions loop start
while(i<90){  //read 90 values from the sensor
  t = millis();
  if (IMU.accelerationAvailable() && t-start > 9) {   //if condition to make sure the data is available and the sampling frequency is 100Hz (10ms intervals)
    IMU.readAcceleration(x, y, z);
    //copying the values to the input buffer tensor for the tflite model
    model_input->data.f[i] = x;
    model_input->data.f[i+89] = y;
    model_input->data.f[i+168] = z;
    i++;
    start = t;
  }
  }
  
  // Run inference
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) {
    error_reporter->Report("Invoke failed on input");
  }

  // Read the predictions from the mode's output buffer (tensor)
  float y_val = model_output->data.f[0];


  // Print value (optional for debug)
  //Serial.println(y_val);

  //saving the values in the predictions array
  predictions[j] = y_val;

}//prediction loop finish

//calculating the average of 10 predictions
avg_pred = average(predictions, 10);

//turning off the red LED, meaning acquisition mode is over. 
digitalWrite(22, HIGH);   

//as defined before, if output is 0, motion is horizental, and if 1, it's vertical
//defining boundries for predictions

//Green LED for horizontal motion
if(avg_pred<0.1){
  digitalWrite(GREEN, LOW);
  while(1);
}

//Blue LED for vertical motion
else if(avg_pred>0.8){
  digitalWrite(BLUE, LOW);
  while(1);
}

//flashing red LED meaning no motion detected 
else{
  while(1){
  digitalWrite(RED,HIGH);
  delay(300);
  digitalWrite(RED,LOW);
  delay(300);
  }
}

}   //main loop finish

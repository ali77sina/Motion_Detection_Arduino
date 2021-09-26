# Motion_Detection_Arduino
Simple piece of code to run a TFlite model on Arduino Nano 33 BLE sense.

Script description as follows: 

1. Data_Acquisition_Arduino (Python): Code to read and save the data from Arduino Nano. This should be compiled when MCU is in acquisition mode.

2. Data_Visualization (Python): Script for simple visualization of data

3. ML_Model (Python): Training the model and convertion to tflite file. 

4. AccAcq.ino is the embedded C putting the MCU in acquisition mode (collecting data for training a model)

5. Code.ino is the script implementing the tflite model on the Arduino Nano.

6. tfliteModel.h is the C array of the learned finalized model.

*NOTE*: The tfliteModel.h and the Code.ino files should be in the same directory, or the path should be altered in the Code.ino file to the location of the tfliteModel.h file. 

For access to data, navigate to the link below in your broswer of choice and download the dataset. Again, make sure that in your ML_Model script, the path is correctly set to have access to these data files.

![image](https://user-images.githubusercontent.com/54308350/134804327-85d416ec-e573-48d6-9d0e-a8f7f47de935.png)

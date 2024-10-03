# S.W.I.M 3D

## Table of Contents

## Introduction

S.W.I.M 3D is a research project that visualizes waves (ultrasonic waves were tested in this experiment) in three dimensions by using a Delta 3D printer, ultrasonic sensors, and real-time data processing. Leveraging the principles from the S.W.I.M (Sequential Wave Imprinting Machine) paper, this systems captures ultrasonic data and represents it visually through synchronized movements of a Delta 3D printer.

## Project Overview

The project involes:

<ul>
    <li>Collecting real-time data (real and complex values) from a transmitter using a receiver and Arduino Uno R3</li>
    <li>Lighting up the LED based on the real and complex values from the Arduino Uno R3</li>
    <li>Processing the data and visualizing it in the polar form using NumPy and Matplotlib</li>
    <li>Controlling the Delta 3D printer (running MainsailOS on a Raspberry Pi 3) to move using Gcode</li>
</ul>

## System Architecture
<img src="https://raw.githubusercontent.com/matoanbach/3d-swim/refs/heads/main/pics/system_architecture.jpeg" width="100%">

## Implementation Details

### Data Acquisition

<ul>
    <li>Analog Signal Reading: Reads analog values from the real and imaging ultrasonic signal inputs on pins A5 and A4</li>
    <li>Signal Processing: 
        <ul>
            <li>Convert analog to digital</li>
            <li>Calculates the amplitude of the signal using the formula: <code>amplitude = sqrt(real^2 + imaginary^2)</code></li>
        </ul>
    </li>
    <li>Serial Communication: Sends the processed data (real, imaginary, amplitude) over serial port to your laptop</li>
</ul>

### Color Encoding

<ul>
    <li>Amplitude-Based Color Coding: Changes the color of the NeoPixel RGB LED based on the amplitude of the ultrasonic signal: 
        <ul>
            <li>No Signal (Amplitude < 2.12 V): LED is turned off (Red, Green, Blue = 0).</li>
            <li>Weak Signal (2.12 V ≤ Amplitude < 4.5 V): LED is blue (Red = 0, Green = 0, Blue = 255).</li>
            <li>Strong Signal (Amplitude ≥ 4.5 V): LED is green (Red = 0, Green = 255, Blue = 0).</li>
        </ul>
    </li>
    <li>Brightness Adjustment: Reduces the brightness of the LED by dividing the RGB values to prevent it from being too bright.</li>
    <li>LED Control: Uses the Adafruit NeoPixel Library to set the color of the LED.</li>
</ul>

## Results
<img src="https://raw.githubusercontent.com/matoanbach/3d-swim/refs/heads/main/pics/result_1.jpeg" width="30%">
<img src="https://raw.githubusercontent.com/matoanbach/3d-swim/refs/heads/main/pics/result_2.jpeg" width="30%">
<img src="https://raw.githubusercontent.com/matoanbach/3d-swim/refs/heads/main/pics/result_3.jpeg" width="30%">
<img src="https://raw.githubusercontent.com/matoanbach/3d-swim/refs/heads/main/pics/result_4.jpeg" width="30%">
<img src="https://raw.githubusercontent.com/matoanbach/3d-swim/refs/heads/main/pics/result_5.jpeg" width="30%">
<img src="https://raw.githubusercontent.com/matoanbach/3d-swim/refs/heads/main/pics/result_6.jpeg" width="30%">

## References

<ul>
    <li>https://www.ameteksi.com/-/media/ameteksi/download_links/documentations/5210/tn1002_the_analog_lock-in_amplifier.pdf?revision=2758ec9f-fab0-488e-85a3-c080bb38d61f</li>
    <li>https://wp.optics.arizona.edu/jpalmer/wp-content/uploads/sites/65/2018/11/LIA_Primer.pdf</li>
    <li>https://fab.cba.mit.edu/classes/863.15/section.CBA/people/Spielberg/Rostock_Delta_Kinematics_3.pdf</li>
    <li>https://doi.org/10.1007/s00170-020-05940-4</li>
    <li>https://doi.org/10.1109/CVPRW.2016.177</li>
    <li>https://doi.org/10.1109/SVR51698.2020.00069</li>
    <li>https://doi.org/10.1109/GEM.2018.8516502</li>
    <li>https://doi.org/10.1109/SENSORS47125.2020.9278809</li>
    <li>https://doi.org/10.1109/SDS49854.2020.9143940</li>
    <li>https://doi.org/10.1109/ICECCME52200.2021.9590939</li>
    <li>https://tspace.library.utoronto.ca/bitstream/1807/91693/3/Yang_Sen_201811_MAS_thesis.pdf</li>
</ul>
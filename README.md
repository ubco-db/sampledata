# Sensor Experiment Sample Data

This repository contains data used for sensor index performance testing. Files are provided in text and binary form. The records in binary form are stored in 512 byte pages. The file convertdata.c converts text to binary form.

## Environmental Data

This environmental data set consists of temperature, humidity, and wind speed readings from various temperature stations in Washington. The two stations used were located at the University of Washington (that sampled every minute) and at Sea-Tac airport (that sampled every hour). The timestamp was
converted into a 4-byte integer representing a UNIX timestamp in seconds. The record size consisted of 16 bytes total including a 4-byte timestamp, 4-byte temperature, 4-byte pressure, and 4-byte wind speed. 

The full data set is available at [https://www-k12.atmos.washington.edu/k12/grayskies/](https://www-k12.atmos.washington.edu/k12/grayskies/). It has been used in previous publications including [MicroHash](https://dl.acm.org/doi/10.5555/1251028.1251031) and [SBITS](https://www.scitepress.org/Link.aspx?doi=10.5220/0010318800920099).

UWA Files:
 - Text: [data/uwa_data_only_2000_500K.txt](data/uwa_data_only_2000_500K.txt)
 - Binary: [data/uwa500K.bin](data/uwa500K.bin)

Sea-Tac Files:
 - Text: [data/seatac_data_100K.txt](data/seatac_data_100K.txt)
 - Binary: [data/sea100K.bin](data/sea100K.bin)
 
## WESAD Data

The [WESAD (wearable stress and affect detection) health data set](https://dl.acm.org/doi/10.1145/3242969.3242985) contains sensor measurements including electrocardiogram (ECG), electrodermal activity, electromyogram, respiration, body temperature, and three-axis acceleration for multiple patients. 500,000 samples from patient S7 were used for testing.

The full data set is available at [https://uni-siegen.sciebo.de/s/HGdUkoNlW1Ub0Gx](https://uni-siegen.sciebo.de/s/HGdUkoNlW1Ub0Gx) ([further information](https://www.eti.uni-siegen.de/ubicomp/home/datasets/icmi18/index.html.en?lang=en)).

Files:
 - Text: [data/S7_respiban_500K.txt](data/S7_respiban_500K.txt)
 - Binary: [data/S7hl500K.bin](data/S7hl500K.bin)

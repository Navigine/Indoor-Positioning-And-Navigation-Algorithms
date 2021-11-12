Message format
============================

Each message consists of one line of json format and describes an individual measurement.
The line types are shown below.

### Type 101 - ACCELEROMETER

```{“timestamp”: <timestamp>, “type”: 101, “values”: [value_x, value_y, value_z]}```

### Type 102 - GYROSCOPE

```{“timestamp”: <timestamp>, “type”: 102, “values”: [value_x, value_y, value_z]}```

### Type 103 - MAGNETOMETER

```{“timestamp”: <timestamp>, “type”: 103, “values”: [value_x, value_y, value_z]}```

### Type 104 -  BAROMETER

```{“timestamp”: <timestamp>, “type”: 104, “values”: [pressure, pressure, pressure]}```

### Type 105 - ORIENTATION

```{“timestamp”: <timestamp>, “type”: 105, “values”: [value_x, value_y, value_z]}```

### Type 106 - LOCATION

```{“timestamp”: <timestamp>, “type”: 106, “values”: [lat, lon, accuracy]}```

### Type 201 - WIFI

```{“timestamp”: <timestamp>, “type”: 201, “bssid”: <bssid>, “rssi”: <rssi>}```

### Type 202 - BLE

```{“timestamp”: <timestamp>, “type”: 202, “bssid”: <bssid>, “rssi”: <rssi>}```

### Type 203 - BEACON

```{“timestamp”: <timestamp>, “type”: 203, “bssid”: <bssid>, “rssi”: <rssi>, “power”: <power>}```

### Type 204 - EDDYSTONE

```{“timestamp”: <timestamp>, “type”: 204, “bssid”: <bssid>, “rssi”: <rssi>, “power”: <power>}```

### Type 205 - WIFI RTT

```{“timestamp”: <timestamp>, “type”: 205, “bssid”: <bssid>, “distance”: <distance>, “stddev”: <stddev>}```

### Type 300 - NMEA

```{“timestamp”: <timestamp>, “type”: 300, “sentence_number”: <sentence_number>, “num_sats”: <num_sats>}```

### Type 900 - Comment

```{“timestamp”: <timestamp>, “type”: 900, “comment”: <comment>}```

# Description of error codes

| Error code | Description                                                                                             |
| :--------: | ------------------------------------------------------------------------------------------------------- |
| 0          | Successful navigation status                                                                            |
| -1         | There are no reference points on the level, or their number is less than three for the KNN algorithm    |
| 4          | No radio measurements for all algorithms or no radio measurements and no steps for the PF_PDR algorithm |
| 7          | Reference point triangulation error for the KNN algorithm                                               |
| 30         | Level not defined                                                                                       |
| 32         | Particle mutation error for the PF algorithm                                                            |
| 33         | Particle sampling error for the PF algorithm                                                            |

## Recommendations for correcting errors

For all cases check that you are using the latest version of your map.

1. For error with code *-1* check that measure.xml is not empty.
2. For errors with codes *4* and *30*, make sure you add beacons to the map and receive data from them (the added beacons should be displayed in DEBUG mode).
3. For error with code *7* check that the reference points do not lie in barriers.
4. Otherwise, write a log along polyline and send it to support@navigine.com.


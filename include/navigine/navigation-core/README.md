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

```{“timestamp”: <timestamp>, “type”: 203, “bssid”: <bssid>, “rssi”: <rssi>, “power” <power>}```

### Type 204 - EDDYSTONE

```{“timestamp”: <timestamp>, “type”: 204, “bssid”: <bssid>, “rssi”: <rssi>, “power” <power>}```

### Type 205 - WIFI RTT

```{“timestamp”: <timestamp>, “type”: 205, “bssid”: <bssid>, “distance”: <distance>, “stddev” <stddev>}```

### Type 300 - NMEA

```{“timestamp”: <timestamp>, “type”: 300, “sentence_number”: <sentence_number>, “num_sats”: <num_sats>}```

### Type 900 - Comment

```{“timestamp”: <timestamp>, “type”: 900, “comment”: <comment>}```

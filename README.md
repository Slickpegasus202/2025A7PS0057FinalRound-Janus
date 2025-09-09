The logic behind the code is pretty simple, we take in data from our gps and 
using the altitude data we can easily find out the state of the system,
there is some accomadation for sensor noise in the code, but it is not very good,
ideally a filter like a kalman filter to filter out the data but time constraints :/
the header file makes it really easy to access data from our sensor

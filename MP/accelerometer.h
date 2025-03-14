#ifndef accelerometer_H__
#define accelerometer_H_

class accelerometer {

public:

	void ADXL345_WRITE(char, char);
	char ADXL345_READ(char);
	int LED_val(int x_axis, int signed_bit);
	
};

#endif /* accelerometer_H_ */
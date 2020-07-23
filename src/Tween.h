#pragma once

/*!
 * Utility class containing static methods for tweening double values.
 */
class Tween {
public:
	static double easeIn(unsigned int tick, int duration) {
		double v = tickToDouble(tick, duration);
		return sin(M_PI/2 * v);
	}
	
	static double easeInOut(unsigned int tick, int duration) {
		double v = tickToDouble(tick, duration);
		return 0.5 * (1.0 - std::cos(v * M_PI));
	}

	static double easeInExpo(unsigned int tick, int duration) {
		double v = tickToDouble(tick, duration);
		return (pow(2, 8 * v) - 1) / 255;
	}

	static double easeOutExpo(unsigned int tick, int duration) {
		double v = tickToDouble(tick, duration);
		return 1 - pow(2, -8 * v);
	}
	
	static double linear(unsigned int tick, int duration) {
		double v = tickToDouble(tick, duration);
		return v;
	}

	static double easeInOutBounce(unsigned int tick, int duration) {
		double v = tickToDouble(tick, duration);
		if (v < 0.5) {
			v = easeInOut(tick, duration/2);
		} else if (v > 0.5) {
			v = easeInOut(tick, duration/2);
			v = 1.0 - v;
		} else {
			return 1.0;
		}
		return v;
	}
	
	static double sineBounce(unsigned int tick, int duration) {
		double v = tickToDouble(tick, duration);
		return std::sin(v * M_PI);
	}
	
	static double linearBounce(unsigned int tick, int duration) {
		double v = tickToDouble(tick, duration);
		if (v > 0.5) v = 1.0 - v;
		v *= 2;
		return v;
	}
private:
	static double tickToDouble(unsigned int tick, int duration) {
		return (tick % duration) / (duration * 1.0);
	}
};
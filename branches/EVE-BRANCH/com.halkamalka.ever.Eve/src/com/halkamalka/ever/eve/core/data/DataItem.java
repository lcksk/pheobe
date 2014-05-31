package com.halkamalka.ever.eve.core.data;

public class DataItem {
	private String name = null;
	private float[] range;
	private float calibration = 0;
	private Integer status = 0;

	public DataItem (String name, float[] range, float calibration, Integer status) {
		this.name = name;
		this.range = range;
		this.calibration = calibration;
		this.status = status;
	}

	public float getMinimum() {
		return range[0];
	}
	
	public float getMaximum() {
		return range[1];
	}
	
	public float getCalibration() {
		return calibration;
	}
	
	public String getName() {
			return name;
	}
	
	public Integer getStatus() {
		return status;
	}
}

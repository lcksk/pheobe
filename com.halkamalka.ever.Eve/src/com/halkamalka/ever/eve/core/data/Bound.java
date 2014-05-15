package com.halkamalka.ever.eve.core.data;

public class Bound {

	private int hash;
	private String name;
	private String major;
	private String minor;
	private Integer status;
	
	public Bound(int hash, String name, String major, String minor,  Integer status) {
		this.hash = hash;
		this.name = name;
		this.major = major;
		this.minor = minor;
		this.status = status;
	}
	
	public int getHash() {
		return hash;
	}
	
	public String getName() {
		return name;
	}
	
	public String getMajor() {
		return major;
	}
	
	public String getMinor() {
		return minor;
	}
	
	public int getStatus() {
		return status;
	}
}

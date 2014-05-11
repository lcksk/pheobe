package com.halkamalka.util;

import java.util.ArrayList;

public final class ThreadPool {

	private static ThreadPool pool = null;
	public static ThreadPool getInstance(){
		if(pool == null){
			pool = new ThreadPool();
		}
		return pool;
	}
	
	private ArrayList<Object> tasks = null;
	private boolean closed = false;
	
	private ThreadPool(){
		tasks = new ArrayList<Object>();
	}
	
	
	
	public synchronized void putTask(Object o){
		if(closed){
			throw new ThreadPoolClosedException();
		}
		tasks.add(o);
		notifyAll();
		
	}
	
	public synchronized Object getTask(){
		while(tasks.size() == 0){
			try{
				wait();
			}
			catch(InterruptedException e){
				e.printStackTrace();
			}
			if(closed){
				throw new ThreadPoolClosedException();
			}
		}
		return tasks.remove(0);
	}
	
	public synchronized void destruct(){
		closed = true;
		notifyAll();
	}
}

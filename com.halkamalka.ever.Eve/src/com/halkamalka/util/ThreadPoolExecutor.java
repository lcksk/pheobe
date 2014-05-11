package com.halkamalka.util;

public class ThreadPoolExecutor {

	private static ThreadPoolExecutor executor = null;
	public static ThreadPoolExecutor getInstance(int numofThread){
		if(executor == null){
			executor = new ThreadPoolExecutor(numofThread);
		}
		return executor;
	}
	
	private boolean closed = false;
	private ThreadPool threadPool = null;
	
	private ThreadPoolExecutor(int numofThread){
	
		threadPool = ThreadPool.getInstance();
		for(int i = 0; i < numofThread; i++){
			Thread thread = new WorkerThread();
			thread.setName("Workder Thread:" + i);
			thread.start();
		}
	}
	
	
	
	public synchronized void execute(Runnable r){
		if(closed){
			throw new ThreadPoolClosedException();
		}
		
		if(threadPool != null){
			threadPool.putTask(r);
		}
	}
	
	
	
	public synchronized void destruct(){
		closed = true;
		if(threadPool != null){
			threadPool.destruct();
		}
	}
	
	
	private class WorkerThread extends Thread{
		public void run(){
			while(!closed){
				((Runnable) threadPool.getTask()).run();
			}
		}
	}
}

package com.halkamalka.ever.eve.core.data;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.net.MalformedURLException;
import java.net.URISyntaxException;
import java.net.URL;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Path;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.logging.Logger;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import javax.xml.parsers.ParserConfigurationException;

import org.apache.commons.compress.archivers.tar.TarArchiveEntry;
import org.apache.commons.compress.archivers.tar.TarArchiveInputStream;
import org.apache.commons.compress.compressors.gzip.GzipCompressorInputStream;
import org.eclipse.core.internal.preferences.Base64;
import org.eclipse.jetty.websocket.api.Session;
import org.json.simple.JSONObject;

import com.halkamalka.ever.eve.preferences.PreferenceConstants;
import com.halkamalka.util.WebsocketListener;
import com.halkamalka.util.WebsocketManager;


public class DataManager implements DataEventSource, WebsocketListener {
	final private static Logger log = Logger.getLogger(DataManager.class.getName());
	public static final int NUMOF_THREAD = 10;
	
	private static DataManager pool = null;
	private ArrayList<Data> data = null;
	private Object lck;
	private Path tmp = null;
	private ArrayList<DataEventListener> dataEventListener = new ArrayList<DataEventListener>();
	final private static String db  =  PreferenceConstants.P_EVE_HOME + File.separator + PreferenceConstants.P_DB_NAME;
	private WebsocketManager client = null;
//	private ThreadPoolExecutor workerthread = null;

	public static DataManager getInstance(){
		if(pool == null){
			pool = new DataManager();
		}
		return pool;
	}

	private DataManager() {
		lck = new Object();
		
		client = WebsocketManager.getInstance();
		client.addWebsocketListener(this);
		try {
			client.connect(PreferenceConstants.getWebsockURI());
		}
		catch (URISyntaxException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
//		workerthread = ThreadPoolExecutor.getInstance(NUMOF_THREAD);
//		workerthread.execute(FileServer.getInstance());
		
		setData(new ArrayList<Data>());

//		new Thread(new Runnable(){
//			@Override
//			public void run() {
//				downloadDB();
//				downloadImage();
//			}
//		}).start();
	}

	static private void deleteTempDirectory(Path path) throws IOException{
		if(path != null) {
			File actual = path.toFile();
			for(File f : actual.listFiles()) {
				log.info(f.getName());
				f.delete();
			}
			Files.deleteIfExists(path);
		}
	}

	public void close() throws IOException {
		if(tmp != null) {
			deleteTempDirectory(tmp);
		}
	}
	
	public Path getTempPath() {
		return tmp;
	}
	
	public ArrayList<Data> getData() {
		return data;
	}

	public void setData(ArrayList<Data> data) {
		this.data = data;
	}
	
	public void add(Data data) {
		log.info("");
		this.data.add(data);
		fireDataEvent(DataStatus.DATA_ADDED, data);
	}
	
	public void remove(Data data) {
		fireDataEvent(DataStatus.DATA_REMOVED, data);
		this.data.remove(data);
	}
	
	public void clear() {
//		for(java.util.Iterator<Data> it = data.iterator();it.hasNext();){
//			Data data = it.next();
//			this.remove(data);
//		}
		this.data.clear();
	}

	public Data findByName(String name) {
		for(Iterator<Data> it = data.iterator(); it.hasNext();) {
			Data data = it.next();
			if(data.getName().contains(name)) {
				return data;
			}
		}
		return null;
	}
	
	public void reload(String path) {
		this.clear();
		fireDataEvent(DataStatus.DATA_RESET, null);
		
		InputStream inputStream = null;
		ZipInputStream zipInputStream = null;
		ZipEntry entry = null;
		ZipEntry entry0 = null;
		
		try {
			if(tmp != null) {
				deleteTempDirectory(tmp);
				tmp = null;
			}
			tmp = Files.createTempDirectory(null);
			tmp.toFile().deleteOnExit();
			
			inputStream = new FileInputStream(path);
			zipInputStream = new ZipInputStream(new BufferedInputStream(inputStream), Charset.forName("MS949"));

			while ((entry = zipInputStream.getNextEntry()) != null) {
				
				//TODO
				if(entry0 == null) {
					entry0 = entry;
				}
				log.info(entry.getName());

				int size;
				byte[] buffer = new byte[2048];
				FileOutputStream outputStream =	new FileOutputStream(tmp.toString() + File.separator +  entry.getName());
				BufferedOutputStream bufferedOutputStream = new BufferedOutputStream(outputStream, buffer.length);

				while ((size = zipInputStream.read(buffer, 0, buffer.length)) != -1) {
					bufferedOutputStream.write(buffer, 0, size);                
				}
				bufferedOutputStream.flush();
				bufferedOutputStream.close();

				//
				if(entry.getName().endsWith("htm") || entry.getName().endsWith("html")) {
//					viewContentProvider.add(tmp.toString(), entry.getName());
					parse(tmp.toString(), entry.getName());
				}
			} // while
			
			// DOM BUILD HERE
			createHTML();

		}
		catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		catch (IllegalArgumentException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
		catch (SecurityException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
		finally {
			if(inputStream != null) {
				try {
					inputStream.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			inputStream = null;
			
			if(zipInputStream != null) {
				try {
					zipInputStream.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			zipInputStream = null;
		}
	}
	
	@Override
	public void addDataEventListener(DataEventListener listener) {
		// TODO Auto-generated method stub
		synchronized(lck) {
			dataEventListener.add(listener);	
		}
	}

	@Override
	public void removeDataEventListener(DataEventListener listener) {
		// TODO Auto-generated method stub
		synchronized(lck) {
			dataEventListener.remove(listener);	
		}		
	}

	@Override
	public void fireDataEvent(DataStatus status, Data data) {
		// TODO Auto-generated method stub
		synchronized (lck) {			
			for(java.util.Iterator<DataEventListener> it = dataEventListener.iterator();it.hasNext();){
				DataEventListener listener = it.next();
				if(status == DataStatus.DATA_REMOVED) {
					listener.dataRemoved(new DataEventObject(data));
				}
				else if(status == DataStatus.DATA_ADDED) {
					listener.dataAdded(new DataEventObject(data));	
				}
				else if(status == DataStatus.DATA_RESET) {
					listener.dataReset();	
				}
				else {
					log.warning("unknown DeviceStatus");
				}
			}
		}
	}

	private void parse(String base, String name) {
		String className = "com.halkamalka.ever.eve.core.data.Data" + name.substring(0, 2);
		Class<?> cls;
		try {
			cls = Class.forName(className);
			Object o = cls.getConstructor(String.class, String.class).newInstance(name, base +  File.separator + name);
			if(o instanceof HtmlParserable) {
				log.info("OK. instance of Html Parserable");
				try {
					((HtmlParserable)o).parse();
					if(o instanceof Data) {
						DataManager.getInstance().add((Data)o);	
					}
				} 
				catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		} 
		catch (ClassNotFoundException | InstantiationException | IllegalAccessException |
				IllegalArgumentException | InvocationTargetException | NoSuchMethodException | SecurityException
				e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
	}

	public void joinEcho() {
		if(client != null && client.isConnected()) {
			JSONObject o = new JSONObject();
			o.put("jsonrpc", "2.0");
			o.put("method", "echo::join");
			o.put("id", "10");
			client.send(o);
		}
	}
	
	public void downloadImage() {
		if(client != null && client.isConnected()) {
			JSONObject o = new JSONObject();
			o.put("jsonrpc", "2.0");
			o.put("method", "product::download_images");
			o.put("id", "10");
			client.send(o);
		}
	}
	
	public void downloadDB(URL url) {
//		String path = db;
//		log.info("path: " + path);
		try {
			File file = new File(db);
			// TODO
			if(!file.exists()) {
				org.apache.commons.io.FileUtils.copyURLToFile(url, file);
			}
		}
		catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public void downloadDB() {
		URL url = null;
		try {
			url = new URL(PreferenceConstants.getDBFileURI());
			downloadDB(url);
		} 
		catch (MalformedURLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public Product[] getProducts(String major, String minor) {
		ArrayList<Product> tmp = new ArrayList<Product>();
		Connection conn = null;
		Statement stmt = null;
		try {
			Class.forName("org.sqlite.JDBC");
			conn = DriverManager.getConnection("jdbc:sqlite:" + db);
			log.info("path: " + db);
			
			stmt = conn.createStatement();
			ResultSet r = stmt.executeQuery( "SELECT * FROM product;" );

			while (r.next() ) {
				String  major0 = r.getString("major");
				String  minor0 = r.getString("minor");
//				String  description = r.getString("description");
				Charset charset = null;
				if(org.apache.commons.exec.OS.isFamilyWindows()) {
					charset = Charset.forName("EUC-KR");
				}
				else {
					charset = Charset.defaultCharset();
				}
				String description = new String(Base64.decode(r.getBytes("description")), charset);
				String  image = r.getString("image");
				File file = new File(getTempPath().toString() + File.separator + image.hashCode());
				OutputStream outputStream = null;
				try {
					if(file.createNewFile()) {
						@SuppressWarnings("restriction")
						byte[] b = Base64.decode(image.getBytes());
						outputStream = new FileOutputStream(file);
						outputStream.write(b);
					}
				} 
				catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				finally {
					if(outputStream != null) {
						try {
							outputStream.close();
						} catch (IOException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
					outputStream = null;
				}
			
				image = file.getPath();
				log.info("description : " + description);
				Product product = new Product(major0, minor0, description, image);
				tmp.add(product);
			}
			r.close();
			stmt.close();
			conn.close();
		} 
		catch (ClassNotFoundException | SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return (Product[]) tmp.toArray(new Product[tmp.size()]);
	}
	
	private void createHTML() {
		ArrayList<Data> lst = getData();
		for(Iterator<Data> it = lst.iterator(); it.hasNext(); ) {
			Data data = it.next();
			if(data.hasAbnormal()) { // TODO
				// build page
				Product[] product = getProducts(null/*TODO*/, null /*TODO*/);
				for(int i = 0; i < product.length; i++) {
//					log.info(product[i].getImage());
				}
		
				String url = data.getName().substring(0, data.getName().indexOf(".htm")) + "_pres.htm";
				DomBuilder dom = new DomBuilder(url);
				try {
					dom.build(data, product);
				} 
				catch (ParserConfigurationException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}

	@Override
	public void onMessage(JSONObject o) {
		// TODO Auto-generated method stub
    	String method = (String) o.get("event");
    	log.info(method);
    	if(method.equals("product::download_images")) {
    		String data = (String) o.get("data");
//    		data = new String(Base64.decode(data.getBytes()));

    		File base = new File(PreferenceConstants.P_EVE_HOME);
    		
    		log.info("base dir : " + base.toString());
    		extractGZip(Base64.decode(data.getBytes()), base);
    	}
    	else if(method.equals("echo::broadcast")) {
    		// TODO : Something has been changed in the server side.
    	}
	}

	@Override
	public void onConnect(Session session) {
		// TODO Auto-generated method stub
		joinEcho();
	}

	@Override
	public void onClose(Session session) {
		// TODO Auto-generated method stub
		
	}
	
	private void extractGZip(byte[] data, File base) {
		TarArchiveInputStream in = null;
			OutputStream out = null;
		try {
			in = new TarArchiveInputStream(new GzipCompressorInputStream(new ByteArrayInputStream(data)));
			TarArchiveEntry entry = null;
			while ((entry = in.getNextTarEntry()) != null) {
				File tmp = new File(base, entry.getName());
				log.info("file : " + tmp.toString());
				if(entry.isDirectory()) {
					org.apache.commons.io.FileUtils.forceMkdir(tmp);
				}
				else {
					if(!tmp.getParentFile().exists()) {
						org.apache.commons.io.FileUtils.forceMkdir(tmp.getParentFile());
					}
					tmp.createNewFile();
					byte[] buf = new byte[(int) entry.getSize()];
					in.read(buf);
					out = new FileOutputStream(tmp);
					out.write(buf);
					out.close();
					out = null;
				}
			}
		} 
		catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		finally {
			if(in!=null) {
				try {
					in.close();
				} 
				catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			if(out != null) {
				try {
					out.close();
				} 
				catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}
}

package com.halkamalka.ever.eve.core.data;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.lang.reflect.InvocationTargetException;
import java.net.MalformedURLException;
import java.net.URI;
import java.net.URL;
import java.net.URLDecoder;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import org.apache.commons.compress.archivers.tar.TarArchiveEntry;
import org.apache.commons.compress.archivers.tar.TarArchiveInputStream;
import org.apache.commons.compress.compressors.gzip.GzipCompressorInputStream;
import org.eclipse.core.internal.preferences.Base64;
import org.eclipse.jetty.websocket.api.Session;
import org.eclipse.ui.PlatformUI;
import org.json.simple.JSONObject;
import org.sqlite.SQLiteConfig;

import com.halkamalka.ever.eve.preferences.PreferenceConstants;
import com.halkamalka.ever.eve.views.NotificationPopUpUI;
import com.halkamalka.util.WebsocketListener;
import com.halkamalka.util.WebsocketManager;




// major - category
// minor - sub-category

@SuppressWarnings("restriction")
public class DataManager implements DataEventSource, WebsocketListener {
	final private static Logger log = Logger.getLogger(DataManager.class.getName());
	public static final int NUMOF_THREAD = 10;
	
	private static DataManager pool = null;
	private ArrayList<Data> data = null;
	private Object lck;
	private Path tmp = null;
	private ArrayList<DataEventListener> dataEventListener = new ArrayList<DataEventListener>();
	final private static String db  =  PreferenceConstants.P_EVE_HOME + File.separator + PreferenceConstants.P_DB_NAME;
	final private static String img  =  PreferenceConstants.P_EVE_HOME + File.separator + PreferenceConstants.P_GZIP_IMG_NAME;

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
		log.setLevel(Level.OFF);
	}

	public static String getDBPath() {
		return db;
	}
	
	static private void deleteTempDirectory(Path path) throws IOException{
		if(path != null) {
			File actual = path.toFile();
			for(File f : actual.listFiles()) {
				log.info(f.getName());
				if(f.isDirectory()) {
					deleteTempDirectory(f.toPath());
				}
				else {
					f.delete();
				}
			}
			Files.deleteIfExists(path);
		}
	}

	public void close() throws IOException {
		log.info("");
		if(tmp != null) {
			deleteTempDirectory(tmp);
		}
	}
	
	public Path getTempPath() {
		if(tmp == null) {
			try {
				tmp = Files.createTempDirectory(null);
			} catch (IOException e) {
				e.printStackTrace();
			}
			tmp.toFile().deleteOnExit();
		}
		return tmp;
	}
	
	public ArrayList<Data> getData() {
		return data;
	}

	public void setData(ArrayList<Data> data) {
		this.data = data;
	}
	
	public void add(Data data) {
		log.info("" + data.getPath());
		synchronized(lck) {
			this.data.add(data);
		}
		fireDataEvent(DataStatus.DATA_ADDED, data);
	}
	
	public void remove(Data data) {
		fireDataEvent(DataStatus.DATA_REMOVED, data);
		synchronized(lck) {
			this.data.remove(data);
		}
	}
	
	public void clear() {
//		for(java.util.Iterator<Data> it = data.iterator();it.hasNext();){
//			Data data = it.next();
//			this.remove(data);
//		}
		synchronized(lck) {
			this.data.clear();
		}
	}

	public Data findByName(String name) {
		synchronized(lck) {
			for(Iterator<Data> it = data.iterator(); it.hasNext();) {
				Data data = it.next();
				if(data.getName().contains(name)) {
					return data;
				}
			}
		}
		return null;
	}
	
	private static void extractZipTo(InputStream inputStream, Path tmp, Charset charset) throws IOException {
		ZipInputStream zipInputStream = null;
		ZipEntry entry = null;
		ZipEntry entry0 = null;

		zipInputStream = new ZipInputStream(new BufferedInputStream(inputStream), charset);

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
//				viewContentProvider.add(tmp.toString(), entry.getName());
				log.info("" + entry.getName());
				parse(tmp.toString(), entry.getName());
			}
		} // while

		if(zipInputStream != null) {
			try {
				zipInputStream.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		zipInputStream = null;
	}
	
	private static void extractZipTo(String in, Path tmp, Charset charset) throws IOException {
		InputStream inputStream = new FileInputStream(in);
		extractZipTo(inputStream, tmp, charset);
		if(inputStream != null) {
			inputStream.close();
			inputStream = null;
		}
	}
	
	private void extractQuantumContents(String path) throws IOException {
		extractZipTo(path, tmp, Charset.forName("MS949"));
	}
	
	public void reload(String path) {
		clear();
		fireDataEvent(DataStatus.DATA_RESET, null);

		try {
			if(tmp != null) {
				deleteTempDirectory(tmp);
				tmp = null;
			}
			tmp = Files.createTempDirectory(null);
			tmp.toFile().deleteOnExit();
			
			extractQuantumContents(path);
	
			// additional DOM BUILD HERE
			createHtmlFile();
			
			fireDataEvent(DataStatus.DATA_LOAD_COMPLATED, null);
		}
		catch (IOException | IllegalArgumentException | SecurityException e) {
			e.printStackTrace();
		}		
	}
	
	@Override
	public void addDataEventListener(DataEventListener listener) {
		synchronized(lck) {
			dataEventListener.add(listener);	
		}
	}

	@Override
	public void removeDataEventListener(DataEventListener listener) {
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
				else if(status == DataStatus.DATA_LOAD_COMPLATED) {
					listener.dataLoadCompleted();
				}
				else {
					log.warning("unknown DeviceStatus");
				}
			}
		}
	}

	private static void parse(String base, String name) {

		String className = null;
		if(name.substring(0, 2).equals("¾çÀÚ")) {
			className = "com.halkamalka.ever.eve.core.data.Data" + "Quantum";
		}
		else {
			className = "com.halkamalka.ever.eve.core.data.Data" + name.substring(0, 2);
		}

		Class<?> cls;
		try {
			cls = Class.forName(className);
			Object o = cls.getConstructor(String.class).newInstance(base +  File.separator + name);
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
				IllegalArgumentException | InvocationTargetException | NoSuchMethodException | SecurityException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
	}

	
	@SuppressWarnings("unchecked")
	public void requestDBVer() {
		if(client != null && client.isConnected()) {
			JSONObject o = new JSONObject();
			o.put("jsonrpc", "2.0");
			o.put("method", "product::db_ver");
			o.put("id", "10");
			client.send(o);
		}
	}
	
	@SuppressWarnings("unchecked")
	public void joinEcho() {
		if(client != null && client.isConnected()) {
			JSONObject o = new JSONObject();
			o.put("jsonrpc", "2.0");
			o.put("method", "echo::join");
			o.put("id", "10");
			client.send(o);
		}
	}
	
	@SuppressWarnings("unchecked")
	public void leaveEcho() {
		if(client != null && client.isConnected()) {
			JSONObject o = new JSONObject();
			o.put("jsonrpc", "2.0");
			o.put("method", "echo::leave");
			o.put("id", "10");
			client.send(o);
		}
	}
	
	@SuppressWarnings("unchecked")
	public void downloadImage() {
		if(client != null && client.isConnected()) {
			JSONObject o = new JSONObject();
			o.put("jsonrpc", "2.0");
			o.put("method", "product::download_images");
			o.put("id", "10");
			client.send(o);
		}
		
		
//		URL url = null;
//		try {
//			url = new URL(PreferenceConstants.getImgFileURI());
//			File file = new File(img);
//			// TODO
//			org.apache.commons.io.FileUtils.copyURLToFile(url, file);
//			File base = new File(PreferenceConstants.P_EVE_HOME);
//			extractGZipTo(file, base);
//		}
//		catch (IOException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
	}
	
	public void downloadDB(URL url) {
		try {
			File file = new File(db);
			// TODO
			org.apache.commons.io.FileUtils.copyURLToFile(url, file);
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

	private String resolveBoundQuery(String major, String minor, Integer status) {
		String query;
		if(major == null && minor == null) {
			query = "SELECT * FROM bound;";
		}
		if(major != null && minor == null){
			query = "SELECT * FROM bound WHERE major=" + "\"" + major + "\"" + ";";
		}
		else if(major != null && minor != null) {
			query = "SELECT * FROM bound WHERE major=" + "\"" + major + "\"" + "minor=" + "\"" + minor + "\"" + ";";
		}
		else if(major != null && minor != null && status != null) {
			query = "SELECT * FROM bound WHERE major=" + "\"" + major + "\"" + "minor=" + "\"" + minor + "\"" + "status=" + status.intValue() +  ";";
		}
		else {
			query = "SELECT * FROM bound;";
		}
		return query;
	}

	public Bound[] getBounds() {
		return getBounds(null, null, null);
	}
	
	public Bound[] getBounds(String major) {
		return getBounds(major, null, null);
	}
	
	public Bound[] getBounds(String major, String minor) {
		return getBounds(major, minor, null);
	}
	
	public Bound[] getBounds(String major, String minor, Integer status) {
		ArrayList<Bound> tmp = new ArrayList<Bound>();
		Connection conn = null;
		Statement stmt = null;
		try {
			Class.forName("org.sqlite.JDBC");
			conn = DriverManager.getConnection("jdbc:sqlite:" + db);
//			log.info("path: " + db);
			
			stmt = conn.createStatement();
			
			String query = resolveBoundQuery(major, minor, status);
			ResultSet r = stmt.executeQuery(query);

			while (r.next() ) {
				String  major0 = r.getString("major");
				String  minor0 = r.getString("minor");
				String name = r.getString("name");
				int hash = r.getInt("hash");
				Integer stat = null;
				switch(r.getInt("status")) {
				case -3:
					stat = Data.LOWEST;
					break;
					
				case -2:
					stat = Data.LOWER;
					break;
					
				case -1:
					stat = Data.LOW;
					break;
					
				case 0:
					stat = Data.NORMAL;
					break;
					
				case 1:
					stat = Data.HIGH;
					break;
					
				case 2:
					stat = Data.HIGHER;
					break;
					
				case 3:
					stat = Data.HIGHEST;
					break;
				}
				
				Charset charset = null;
				if(org.apache.commons.exec.OS.isFamilyWindows()) {
					charset = StandardCharsets.UTF_8;
				}
				else {
					charset = Charset.defaultCharset();
				}

				Bound bound = new Bound(hash, name, major0, minor0, stat);
				tmp.add(bound);
			}
			r.close();
			stmt.close();
			conn.close();
		} 
		catch (ClassNotFoundException | SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return tmp.toArray(new Bound[tmp.size()]);
	}

	public Product getLatestProduct() {
		Product[] product = getProducts(null, null, 1);
		if(product != null && product.length > 0) {
			return product[0];
		}
		return null; 
	}

	public Product[] getProducts() {
		return getProducts(null, null, 0);
	}
	
	public Product[] getProducts(String productName, Integer nameHash) {
		return getProducts(productName, nameHash, 0);
	}
	
	private String resolveProductQuery(String name, Integer hash, int count) {
		String query = null;
		
		if(hash != null) {
			query = "SELECT * FROM product WHERE hash=" +  hash.intValue() + " ORDER BY date(date) DESC";
			return query;
		}
		
		if(name == null) {
			query = "SELECT * FROM product" + " ORDER BY date(date) DESC";
		}
		else {
			query = "SELECT * FROM product WHERE name=" + "\"" + name + "\"" + " ORDER BY date(date) DESC"; 
		}
		if(count > 0) {
			query += " LIMIT " + count;
		}
		query += ";";
		return query;
	}
	
	public Product[] getProducts(String productName, Integer nameHash, int count) {
		ArrayList<Product> tmp = new ArrayList<Product>();
		Connection conn = null;
		Statement stmt = null;
		try {
			Class.forName("org.sqlite.JDBC");
			SQLiteConfig config = new SQLiteConfig();
			config.setEncoding(SQLiteConfig.Encoding.UTF8);
			conn = DriverManager.getConnection("jdbc:sqlite:" + db);
			log.info("path: " + db);
			
			stmt = conn.createStatement();
			
			String query = resolveProductQuery(productName, nameHash, count);
			
			ResultSet r = stmt.executeQuery(query);

			while (r.next() ) {
				String name = r.getString("name");
				int price = r.getInt("price0");
				int hash = r.getInt("hash");
//				String  description = r.getString("description");
				Charset charset = null;
				if(org.apache.commons.exec.OS.isFamilyWindows()) {
					charset = StandardCharsets.UTF_8;
				}
				else {
					charset = Charset.defaultCharset();
				}
				@SuppressWarnings("restriction")
				String description = new String(Base64.decode(r.getBytes("description")), charset);
				if(org.apache.commons.exec.OS.isFamilyWindows()) {
					try {
						description = URLDecoder.decode(description, charset.name());
					} catch (UnsupportedEncodingException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}

				String  image = r.getString("thumbnail");
				
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
				Product product = new Product(hash, name, description, image, price);
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
		return tmp.toArray(new Product[tmp.size()]);
	}
	
//	private void createHTML() {
//		ArrayList<Data> lst = getData();
//		for(Iterator<Data> it = lst.iterator(); it.hasNext(); ) {
//			Data data = it.next();
//			if(data.hasAbnormal()) { // TODO
//				// build page
//				Product[] product = getProducts(null/*TODO*/, null /*TODO*/);
//				for(int i = 0; i < product.length; i++) {
////					log.info(product[i].getImage());
//				}
//		
//				String url = data.getName().substring(0, data.getName().indexOf(".htm")) + "_pres.htm";
//				DomBuilder dom = new DomBuilder(url);
//				try {
//					dom.build(data, product);
//				} 
//				catch (ParserConfigurationException e) {
//					// TODO Auto-generated catch block
//					e.printStackTrace();
//				}
//			}
//		}
//	}

	private void createHtmlFile() {
		batchJQuery(getTempPath()); // TODO
		ArrayList<Data> lst = getData();
		for(Iterator<Data> it = lst.iterator(); it.hasNext(); ) {
			Data data = it.next();
			if(data.hasAbnormal()) { // TODO
				// build page
				
				String url = data.getFileName().substring(0, data.getFileName().indexOf(".htm")) + "_pres.htm";
				DomBuilder dom = new DomBuilder(url);
				try {
					log.info("$$$$$$$" + data.getName());
					DataItem item = data.get(data.getName());
					if(item != null) {
//						log.info("" + item.getName());
					}
//					Bound[] bound = getBounds(data.getMajor(), data.get(data.getName()).getName());
//					Bound[] bound = getBounds(data.getName(), data.get(data.getName()).getName());
//					Bound[] bound = getBounds(data.getName());
					Bound[] bound = getBounds(data.getName());
					dom.build(data, bound);
				} 
				catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}
	
	@Override
	public void onMessage(JSONObject o) {
    	String method = (String) o.get("event");
    	log.info(method);
    	if(method.equals("product::download_images")) {
    		String data = (String) o.get("data"); 
    		File base = new File(PreferenceConstants.P_EVE_HOME);
    		
//    		log.info("base dir : " + base.toString());
    		extractGZipTo(Base64.decode(data.getBytes(StandardCharsets.UTF_8)), base);
    	}
    	else if(method.equals("echo::broadcast")) {
    		// TODO : Something has been changed in the server side.
    		log.info(method);
    		String msg = (String) o.get("msg");
    		if(msg != null && msg.equals("db-update")) {
        		PlatformUI.getWorkbench().getDisplay().asyncExec(new Runnable(){
        			@Override
        			public void run() {
        				// TODO Auto-generated method stub
        				// Change to use handler
        				DataManager.getInstance().downloadImage();
        				DataManager.getInstance().downloadDB();
//        				IHandlerService service = (IHandlerService) PlatformUI.getWorkbench().getService(IHandlerService.class);
//        				try {
//							service.executeCommand("com.halkamalka.ever.Eve.commands.updateDBCommand", null);
//						} catch (ExecutionException e) {
//							// TODO Auto-generated catch block
//							e.printStackTrace();
//						} catch (NotDefinedException e) {
//							// TODO Auto-generated catch block
//							e.printStackTrace();
//						} catch (NotEnabledException e) {
//							// TODO Auto-generated catch block
//							e.printStackTrace();
//						} catch (NotHandledException e) {
//							// TODO Auto-generated catch block
//							e.printStackTrace();
//						}
        				// Change to use handler
        				NotificationPopUpUI popup = new  NotificationPopUpUI(PlatformUI.getWorkbench().getDisplay());
        		        popup.open();
        			}
        		});
    		}
    	}
	}

	@Override
	public void onConnect(Session session) {
		// TODO Auto-generated method stub
		log.info("");
		joinEcho();
	}

	@Override
	public void onClose(Session session) {
		// TODO Auto-generated method stub
		log.info("");
		leaveEcho();
	}
	
	public static void extractGZipTo(File file, File base) {
		TarArchiveInputStream in = null;
		OutputStream out = null;
		try {
			in = new TarArchiveInputStream(new GzipCompressorInputStream(new FileInputStream(file)), "utf-8");
			TarArchiveEntry entry = null;
			while ((entry = in.getNextTarEntry()) != null) {
				File tmp = new File(base, entry.getName());
	//			log.info("file : " + tmp.toString());
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
	
	public static void extractGZipTo(byte[] data, File base) {
		TarArchiveInputStream in = null;
			OutputStream out = null;
		try {
			in = new TarArchiveInputStream(new GzipCompressorInputStream(new ByteArrayInputStream(data)), "utf-8");
			TarArchiveEntry entry = null;
			while ((entry = in.getNextTarEntry()) != null) {
				File tmp = new File(base, entry.getName());
//				log.info("file : " + tmp.toString());
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
	
	public Data getFirstData() {
		if((data == null) || (data.size() == 0)) {
			return null;
		}
		return data.get(0);
	}
	
	public Data getLastData() {
		if((data == null) || (data.size() == 0)) {
			return null;
		}
		return data.get(data.size()-1);
	}
	
	public boolean dbExists() {
		return new File(db).exists();
	}
	
	private void batchJQuery(Path tmp) {
		InputStream in = getClass().getResourceAsStream("/res/jslib.tar.gz"); // TODO

		try {
			byte[] b = new byte[in.available()];
			for(int offset=0,n=0; offset < b.length; offset+=n) {
				n = in.read(b, offset, b.length - offset);
			}
			DataManager.extractGZipTo(b, tmp.toFile());
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		finally {
			if(in != null) {
				try {
					in.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				in = null;
			}
		}
	}
}

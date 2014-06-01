package com.halkamalka.ever.eve.views;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.logging.Level;
import java.util.logging.Logger;

import org.apache.commons.io.FileUtils;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.commands.NotEnabledException;
import org.eclipse.core.commands.NotHandledException;
import org.eclipse.core.commands.common.NotDefinedException;
import org.eclipse.core.internal.preferences.Base64;
import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.jetty.websocket.api.Session;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.ITreeSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerSorter;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.DropTargetEvent;
import org.eclipse.swt.dnd.DropTargetListener;
import org.eclipse.swt.dnd.FileTransfer;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.handlers.IHandlerService;
import org.eclipse.ui.part.ViewPart;
import org.json.simple.JSONObject;

import com.halkamalka.ever.eve.core.data.Data;
import com.halkamalka.ever.eve.core.data.DataEventListener;
import com.halkamalka.ever.eve.core.data.DataEventObject;
import com.halkamalka.ever.eve.core.data.DataItem;
import com.halkamalka.ever.eve.core.data.DataManager;
import com.halkamalka.util.CustomSharedImage;
import com.halkamalka.util.WebsocketListener;
import com.halkamalka.util.WebsocketManager;

public class CategoryViewPart extends ViewPart implements DataEventListener, DropTargetListener, WebsocketListener {

	public static final String ID = "com.halkamalka.ever.eve.views.CategoryViewPart";
	private TreeViewer viewer;
	private ViewContentProvider viewContentProvider;
	
	final private static Logger log = Logger.getLogger(CategoryViewPart.class.getName());
	
	public CategoryViewPart() {
		log.setLevel(Level.OFF);
		DataManager.getInstance().addDataEventListener(this);
		WebsocketManager.getInstance().addWebsocketListener(this);
	}


	@Override
	public void createPartControl(Composite parent) {

		// connect to websocket server
		connectCommand();

		viewer = new TreeViewer(parent, SWT.MULTI | SWT.H_SCROLL | SWT.V_SCROLL);
		viewContentProvider = new ViewContentProvider();
		viewer.setContentProvider(viewContentProvider);
		viewer.setLabelProvider(new ViewLabelProvider());
		viewer.setSorter(new NameSorter());
		viewer.setInput(getViewSite());
		getViewSite().setSelectionProvider(viewer);
		
		viewer.addSelectionChangedListener(new ISelectionChangedListener() {
			@Override
			public void selectionChanged(SelectionChangedEvent event) {
				ITreeSelection selection = (ITreeSelection)event.getSelection();
				log.info(selection.getFirstElement() + " selected");
				
				//TODO
				TreeObject object = (TreeObject)selection.getFirstElement();
				if(object == null) {
					return;
				}
				
//				IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
				IWorkbenchPage page = getViewSite().getPage(); 
				if(page == null) {
					return;
				}
				try {
					page.showView(BrowserViewPart.ID);
					IViewPart part = page.findView(BrowserViewPart.ID);
					if(!(part instanceof BrowserViewPart)) {
						return;
					}
					BrowserViewPart browser = (BrowserViewPart) part;
//					URI uri = URI.create(filename);
//					String url = object.getBase() + File.separator + object.getName();
					String url = object.getPath();

					log.info( url);
					browser.setUrl( url);
					browser.setTitleLabel(object.toString());
				} catch (PartInitException e) {
					e.printStackTrace();
				}
			}			
		});
		
		viewer.addDoubleClickListener(new IDoubleClickListener(){
			@Override
			public void doubleClick(DoubleClickEvent event) {
				// TODO Auto-generated method stub
			}
		});

		viewer.addDropSupport(DND.DROP_COPY | DND.DROP_MOVE, new Transfer[]{FileTransfer.getInstance()}, this);
	}

	@Override
	public void setFocus() {
		viewer.getControl().setFocus();
	}

	
	class TreeObject implements IAdaptable {
		private String name;
//		private String base;
		private TreeParent parent;
		private Data data;
		
		public TreeObject(String name) {
			this.name = name;
		}
//
//		public TreeObject(String base, String name) {
//			this.name = name;
//			this.base = base;
//		}
		
		public TreeObject(Data data) {
			this.data = data;
		}

		public String getName() {
			if(data==null) {
				return name;
			}
			return data.getName();
		}

		public String getFileName() {
			if(data==null) {
				return name;
			}
			return data.getFileName();
		}
		
//		public String getBase() {
//			return base;
//		}
		
		public Data getData() {
			return data;
		}
		
		public String getPath() {
			if(data==null) {
//				return "";
				return parent.getData().getPrescriptionPath();
			}
			return data.getPath();
		}
		
		public void setParent(TreeParent parent) {
			this.parent = parent;
		}

		public TreeParent getParent() {
			return parent;
		}

		public String toString() {
			if(getFileName().endsWith(".htm")) {
				return getFileName().substring(0, getFileName().indexOf(".htm"));
			}
			return getName();
//			return getName();
		}

		@SuppressWarnings("rawtypes")
		public Object getAdapter(Class key) {
			return null;
		}
	}

	class TreeParent extends TreeObject {
		public TreeParent(String name) {
			super(name);
			// TODO Auto-generated constructor stub
			children = new ArrayList<TreeObject>();
		}

		public TreeParent(Data data) {
			super(data);
			children = new ArrayList<TreeObject>();
		}

		private ArrayList<TreeObject> children;
//		@SuppressWarnings("rawtypes")
//		Device device = null;

//		public TreeParent(String name, @SuppressWarnings("rawtypes") Device device) {
//			super(name);
//			children = new ArrayList<TreeObject>();
//			this.device = device;
//		}

		public void addChild(TreeObject child) {
			children.add(child);
			child.setParent(this);
		}

		public void removeChild(TreeObject child) {
			children.remove(child);
			child.setParent(null);
		}
		
		public void removeChild(Data data) {
			for(Iterator<TreeObject> it = children.iterator(); it.hasNext();) {
				TreeObject o = it.next();
				if(o.getData().equals(data)) {
					children.remove(o);
					break;
				}
			}
		}

		public TreeObject[] getChildren() {
			return (TreeObject[]) children.toArray(new TreeObject[children
					.size()]);
		}

		public boolean hasChildren() {
			return children.size() > 0;
		}
	}
	
	class ViewContentProvider implements IStructuredContentProvider, ITreeContentProvider {

		private TreeParent invisibleRoot;

		public void inputChanged(Viewer v, Object oldInput, Object newInput) {
		}

		public void dispose() {
		}

		public Object[] getElements(Object parent) {
			if (parent.equals(getViewSite())) {
				if (invisibleRoot == null) {
					reinitialize();
				}
				return getChildren(invisibleRoot);
			}
			return getChildren(parent);
		}

		public Object getParent(Object child) {
			if (child instanceof TreeObject) {
				return ((TreeObject) child).getParent();
			}
			return null;
		}

		public Object[] getChildren(Object parent) {
			if (parent instanceof TreeParent) {
				return ((TreeParent) parent).getChildren();
			}
			return new Object[0];
		}

		public boolean hasChildren(Object parent) {
			if (parent instanceof TreeParent)
				return ((TreeParent) parent).hasChildren();
			return false;
		}

		/*
		 * We will set up a dummy model to initialize tree heararchy. In a real
		 * code, you will connect to a real model and expose its hierarchy.
		 */
		public void reinitialize() {
			invisibleRoot = new TreeParent((Data) null);
		}

//		public void add(String item) {
//			TreeObject object = new TreeObject(item);
//			invisibleRoot.addChild(object);			
//			PlatformUI.getWorkbench().getDisplay().asyncExec(new Runnable(){
//				@Override
//				public void run() {
//					// TODO Auto-generated method stub
//					viewer.refresh();
//					viewer.expandToLevel(2);
//				}				
//			});		
//		}
//		
//		public void add(String base, String item) {
//			TreeObject object = new TreeObject(base, item);
//			invisibleRoot.addChild(object);			
//			PlatformUI.getWorkbench().getDisplay().asyncExec(new Runnable(){
//				@Override
//				public void run() {
//					// TODO Auto-generated method stub
//					viewer.refresh();
//					viewer.expandToLevel(2);
//				}				
//			});		
//		}
		public void add(Data item) {
			DataItem[] lst = item.findAbnormal(); 
			TreeObject object = null;
			if(lst.length > 0) {
				object = new TreeParent(item);
				for(int i = 0; i < lst.length; i++) {
					String key = (String) lst[i].getName();
					log.info("<><><>" + key + ":" + item.get(key));
					((TreeParent) object).addChild(new TreeObject(key));
				}
			}
			else {
				object = new TreeObject(item);
			}

			invisibleRoot.addChild(object);			
			PlatformUI.getWorkbench().getDisplay().asyncExec(new Runnable(){
				@Override
				public void run() {
					viewer.refresh();
//					viewer.expandToLevel(2);
				}				
			});		
		}
		
		public void remove(Data item) {
			invisibleRoot.removeChild(item);		
//			TreeObject[] object = invisibleRoot.getChildren();
//			for(int i = 0; i < object.length; i++) {
//				if(object[i].getName().compareTo(item.getName())==0) {
//					invisibleRoot.removeChild(object[i]);
//				}
//			}
//			PlatformUI.getWorkbench().getDisplay().asyncExec(new Runnable(){
//				@Override
//				public void run() {
//					// TODO Auto-generated method stub
//					viewer.refresh();
//					viewer.expandToLevel(2);
//				}				
//			});	
		}
		
		// TODO
		public void remove(TreeObject item) {
			
		}
	}


	class ViewLabelProvider extends LabelProvider {
		public String getText(Object obj) {
			return obj.toString();
		}

		public Image getImage(Object obj) {
//			String imageKey = ISharedImages.IMG_OBJ_FILE;
			if (obj instanceof TreeParent) {
//				imageKey = ISharedImages.IMG_OBJ_FOLDER;				
				return CustomSharedImage.getInstance().getImage(CustomSharedImage.IMG_WARN);
			}
			TreeObject o = (TreeObject) obj;
			TreeParent p = o.getParent();
			Data data = p.getData();
			if((data != null) && data.hasAbnormal()) {
				return CustomSharedImage.getInstance().getImage(CustomSharedImage.IMG_MSG_WARN);
			}
//			if(o.getData().hasAbnormal()) {
//			if(o.getData().hasAbnormal()) {
//				
//			}
			return CustomSharedImage.getInstance().getImage(CustomSharedImage.IMG_PASSED);
		}
	}

	class NameSorter extends ViewerSorter {
	}

	@Override
	public void dataAdded(DataEventObject e) {
		log.info("");
		Object object = e.getSource();
		if(object instanceof Data) {
			log.info("Data");
			Data data = (Data) object;
			log.info(data.getName());
			viewContentProvider.add(data);
			
//			IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
//			if(page == null) {
//				return;
//			}
//			
//			try {
//				page.showView(BrowserViewPart.ID);
//				IViewPart part = page.findView(BrowserViewPart.ID);
//				if(!(part instanceof BrowserViewPart)) {
//					return;
//				}
//				BrowserViewPart browser = (BrowserViewPart) part;
//				browser.setUrl(data.getPath());
//				browser.setTitleLabel(data.getName().substring(0, data.getName().indexOf(".htm")));
//				
//			} catch (PartInitException ex) {
//				ex.printStackTrace();
//			}
		}
	}

	@Override
	public void dataRemoved(DataEventObject e) {
		Object object = e.getSource();
		if(object instanceof Data) {
			Data data = (Data) object;
			viewContentProvider.remove(data);
		}
	}

	@Override
	public void dataReset() {
		viewContentProvider.reinitialize();
	}


	@Override
	public void dragEnter(DropTargetEvent event) {
		if(event.detail == DND.DROP_MOVE || event.detail == DND.DROP_DEFAULT) {
			if((event.operations & DND.DROP_COPY) != 0) {
				event.detail = DND.DROP_COPY;
			}
			else {
				event.detail = DND.DROP_NONE;
			}
		}
	}


	@Override
	public void dragOperationChanged(DropTargetEvent event) {
		// TODO Auto-generated method stub
	}


	@Override
	public void dragOver(DropTargetEvent event) {
		// TODO Auto-generated method stub
	}


	@Override
	public void drop(DropTargetEvent event) {
		if(FileTransfer.getInstance().isSupportedType(event.currentDataType)) {
			log.info("");
			event.detail = DND.DROP_COPY;
			String[] files = (String[]) event.data;
			for(int i = 0; i < files.length; i++) {
				log.info(files[i]);
			}
			if(files.length > 0) {
				DataManager.getInstance().reload(files[0]);
			}
		}
		else {
			event.detail = DND.DROP_NONE;
		}
	}


	@Override
	public void dropAccept(DropTargetEvent event) {
	}


	@Override
	public void dragLeave(DropTargetEvent event) {		
	}
	
	private void updateDBCommand() {
		IHandlerService handlerService = (IHandlerService) getSite().getService(IHandlerService.class);
		try {
			handlerService.executeCommand("com.halkamalka.ever.Eve.commands.updateDBCommand", null);
		} 
		catch (ExecutionException | NotDefinedException | NotEnabledException | NotHandledException e) {
			e.printStackTrace();
		} 
	}

	private void connectCommand() {
		IHandlerService handlerService = (IHandlerService) getSite().getService(IHandlerService.class);
		try {
			handlerService.executeCommand("com.halkamalka.ever.Eve.commands.connectCommand", null);
		} 
		catch (ExecutionException | NotDefinedException | NotEnabledException | NotHandledException e) {
			e.printStackTrace();
		} 
	}

	@Override
	public void dataLoadCompleted() {
		log.info("");
		getSite().getShell().getDisplay().asyncExec(new Runnable(){
			@Override
			public void run() {
				// TODO Auto-generated method stub
				IWorkbenchPage page = getViewSite().getPage(); 
				if(page == null) {
					return;
				}
				
				try {
					page.showView(BrowserViewPart.ID);
					IViewPart part = page.findView(BrowserViewPart.ID);
					if(!(part instanceof BrowserViewPart)) {
						return;
					}
					BrowserViewPart browser = (BrowserViewPart) part;
					Data data = DataManager.getInstance().getLastData();
					browser.setUrl(data.getPath());
//					browser.setTitleLabel(data.getName().substring(0, data.getName().indexOf(".htm")));
					browser.setTitleLabel(data.getName());
					
				} catch (PartInitException ex) {
					ex.printStackTrace();
				}
			}
		});
	}


	@SuppressWarnings("restriction")
	private String getLocalDBHash() throws IOException{
		byte[] sha1 = Base64.encode(org.apache.commons.codec.digest.DigestUtils.sha1(FileUtils.readFileToByteArray(new File(DataManager.getDBPath()))));
		return new String(sha1);
	}

	@Override
	public void onMessage(JSONObject o) {
		// TODO Auto-generated method stub
    	String method = (String) o.get("event");
    	log.info(method);
    	if(method.equals("product::db_ver")) {
    		String remote = (String) o.get("db_ver"); 
//    		log.info("base dir : " + base.toString());
    		log.info(remote);
    		try {
//				log.info(getLocalDBHash());
    			if(new File(DataManager.getDBPath()).exists()) {
    	    		if(remote.compareTo(getLocalDBHash()) != 0) {
    	    			updateDBCommand();
    	    		}
    			}
    			else {
	    			updateDBCommand();
    			}
			}
    		catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
    	}
	}


	@Override
	public void onConnect(Session session) {
		log.info("");
		if(!DataManager.getInstance().dbExists()) {
			updateDBCommand();
		}
		
		DataManager.getInstance().requestDBVer();
	}


	@Override
	public void onClose(Session session) {
		// TODO Auto-generated method stub
		
	}
	
}

package com.halkamalka.ever.eve.core.data;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Path;
import java.util.logging.Logger;

import javax.xml.parsers.ParserConfigurationException;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;

import com.halkamalka.ever.eve.preferences.PreferenceConstants;

public class DomBuilder {
	final private static Logger log = Logger.getLogger(DomBuilder.class.getName());
	Document html;
	String name;

	public DomBuilder(String name) {
		this.name = name;
	}

	public void build(Data data, Bound[] bound) throws ParserConfigurationException {

		log.info("num of bounds" + bound.length);
		Document doc = Jsoup.parse(loadTemplate());
		// Document sub = Jsoup.parse(loadSubTemplate());

		if (org.apache.commons.exec.OS.isFamilyWindows()) {
			// replace encoding to euc-kr
			Element meta = doc.select("meta").first();
			meta.attr("content", "text/html; charset=euc-kr");
		}

		Element prescription = doc.select("#prescription").first();
		if (data.getPrescription() != null) {
			prescription.text(data.getPrescription());
		}

		Element major = doc.select("#major").first();
		major.text(data.getName());

		Element wrapper = doc.select("#wrapper").first();

		if (data.hasAbnormal()) {

			// Element productList = createRecommendListTemplate(doc);
			// wrapper.appendChild(productList);

			DataItem[] abnormalItem = data.findAbnormal();
			log.info("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");

			Integer a;
			for (a = 0; a < abnormalItem.length; a++) {
				log.info("abnormal minor : " + abnormalItem[a].getName());

				for (int i = 0; i < bound.length; i++) {
					Bound b = bound[i];
					Product[] products = DataManager.getInstance().getProducts(b.getProductName(), null);

					log.info("bound minor : " + b.getMinor());
					for (int j = 0; j < products.length; j++) {

						if (abnormalItem[a].getName().equals(bound[i].getMinor())) {

							// status > 0
							if ((abnormalItem[a].getStatus() > 0) && (bound[i].getStatus() > 0)) {

								// RECOMMEND LIST CONTAINER
								Element recommendListContainer = doc.createElement("div");
								recommendListContainer.attr("class", "productFold");
								String recommendListContainerStyle = "position:relative; width: 100%; float: left; background-color:#e0e0e0; border-top: 1px solid white; font-size:13px; cursor:pointer;";
								recommendListContainer.attr("style", recommendListContainerStyle);
								wrapper.appendChild(recommendListContainer);

								// MINOR
								String _minor = abnormalItem[a].getName();
								Element _minorDom = doc.createElement("div");
								String _minorDomStyle = "position:relative; width:20%; float:left; height:50px; line-height:50px; padding-top:5px; word-wrap: break-word;";
								_minorDom.attr("style", _minorDomStyle);
								_minorDom.html(_minor);
								recommendListContainer.appendChild(_minorDom);

								// STATUS
								Integer _status = abnormalItem[a].getStatus();
								Element _statusDom = createStatus(doc, _status);
								recommendListContainer.appendChild(_statusDom);

								// PRODUCT IMAGE
								log.info("******************product image****************** : "+ products[j].getImage());
								String _productImage = products[j].getImage();
//								String _productImage = products[j].getImageName();
								Element _productImageDom = createProductImage(doc, _productImage);
								recommendListContainer.appendChild(_productImageDom);
								//
								// //PRODUCT NAME
								log.info("******************product name****************** : "+ products[j].getName());
								String _productName = products[j].getName();
								Element _productNameDom = createProductName(doc, _productName);
								recommendListContainer.appendChild(_productNameDom);
								//
								// //PRODUCT PRICE
								log.info("******************product price****************** : "+ products[j].getPrice0());
								Integer _productPrice = products[j].getPrice0();
								Element _productPriceDom = createProductPrice(doc, _productPrice);
								recommendListContainer.appendChild(_productPriceDom);

								// PRODUCT DESCRIPTION TOGGLE
								// Element _productDescriptionToggleDom =
								// createProductDescriptionToggle(doc);
								// recommendListContainer.appendChild(_productDescriptionToggleDom);

								// PRODUCT DESCRIPTION
								log.info("******************product description****************** : "+ products[j].getDescription());
								String _productDescription = products[j].getDescription();
								Element _productDescriptionDom = createProductDesciption(doc, _productDescription);
								recommendListContainer.appendChild(_productDescriptionDom);

								// status < 0
							} else if ((abnormalItem[a].getStatus() < 0) && (bound[i].getStatus() < 0)) {

								// RECOMMEND LIST CONTAINER
								Element recommendListContainer = doc.createElement("div");
								recommendListContainer.attr("class", "productFold");
								String recommendListContainerStyle = "position:relative; width: 100%; float: left; background-color:#e0e0e0; border-top: 1px solid white; font-size:13px; cursor:pointer;";
								recommendListContainer.attr("style", recommendListContainerStyle);
								wrapper.appendChild(recommendListContainer);

								// MINOR
								String _minor = abnormalItem[a].getName();
								Element _minorDom = doc.createElement("div");
								String _minorDomStyle = "position:relative; width:20%; float:left; height:50px; line-height:50px; padding-top:5px; word-wrap: break-word;";
								_minorDom.attr("style", _minorDomStyle);
								_minorDom.html(_minor);
								recommendListContainer.appendChild(_minorDom);

								// STATUS
								Integer _status = abnormalItem[a].getStatus();
								Element _statusDom = createStatus(doc, _status);
								recommendListContainer.appendChild(_statusDom);

								// PRODUCT IMAGE
								log.info("******************product image****************** : "+ products[j].getImage());
								String _productImage = products[j].getImage();
//								String _productImage = products[j].getImageName();
								Element _productImageDom = createProductImage(doc, _productImage);
								recommendListContainer.appendChild(_productImageDom);
								//
								// //PRODUCT NAME
								log.info("******************product name****************** : "+ products[j].getName());
								String _productName = products[j].getName();
								Element _productNameDom = createProductName(doc, _productName);
								recommendListContainer.appendChild(_productNameDom);
								//
								// //PRODUCT PRICE
								log.info("******************product price****************** : "+ products[j].getPrice0());
								Integer _productPrice = products[j].getPrice0();
								Element _productPriceDom = createProductPrice(doc, _productPrice);
								recommendListContainer.appendChild(_productPriceDom);

								// PRODUCT DESCRIPTION TOGGLE
								// Element _productDescriptionToggleDom =
								// createProductDescriptionToggle(doc);
								// recommendListContainer.appendChild(_productDescriptionToggleDom);

								// PRODUCT DESCRIPTION
								log.info("******************product description****************** : "+ products[j].getDescription());
								String _productDescription = products[j].getDescription();
								Element _productDescriptionDom = createProductDesciption(doc, _productDescription);
								recommendListContainer.appendChild(_productDescriptionDom);
							}
						}
					}
					// System.out.println(doc.html());
				}
				save(DataManager.getInstance().getTempPath(), doc.html());
			}
		}
	}

	private Element createRecommendListTemplate(Document doc) {

		Element template = doc.createElement("div");
		template.attr("style", "position: relative; left:0;/* padding:10px; */ width: 100%; margin-top: 30px;");

		Element minor = doc.createElement("div");
		minor.attr("style", "position: relative; width: 20%; float: left; height:30px; line-height:30px; background-color: yellow;");
		minor.html("<a>측정 항목</a>");
		template.appendChild(minor);

		Element status = doc.createElement("div");
		status.attr("style", "position: relative; width: 30%; float: left; height:30px; line-height:30px; background-color: yellow;");
		status.html("<a>상 태</a>");
		template.appendChild(status);

		Element recommend = doc.createElement("div");
		recommend
				.attr("style",
						"position: relative; width: 20%; float: left; height:30px; line-height:30px; background-color: yellow;");
		recommend.html("<a>추천 제품</a>");
		template.appendChild(recommend);

		Element product = doc.createElement("div");
		product.attr("style", "position: relative; width: 15%; float: left; height:30px; line-height:30px; background-color: yellow;");
		product.html("<a>품 명</a>");
		template.appendChild(product);

		Element price = doc.createElement("div");
		price.attr("style", "position: relative; width: 15%; float: left; height:30px; line-height:30px; background-color: yellow;");
		price.html("<a>가 격</a>");
		template.appendChild(price);

		// Element detail = doc.createElement("div");
		// detail.attr("style",
		// "position: relative; width: 10%; float: left; height:30px; line-height:30px; background-color: yellow;");
		// detail.html("<a>상세 보기</a>");
		// template.appendChild(detail);

		return template;
	}

	private Element createStatus(Document doc, Integer status) {

		Element _statusDom = doc.createElement("div");
		String _statusDomStyle = "position:relative; width:30%; float:left; height:50px; line-height:50px; padding-top:10px";
		_statusDom.attr("style", _statusDomStyle);

		Element _statusImageAlign = doc.createElement("a");
		_statusImageAlign.attr("style", "text-align:center;");
		_statusDom.appendChild(_statusImageAlign);

		Element _statusImageTag = doc.createElement("img");
		_statusImageTag.attr("width", "80%");
		_statusImageTag.attr("height", "30px");

		switch (status) {
		case -3:
			_statusImageTag.attr("src", "YC01.gif");
			break;
		case -2:
			_statusImageTag.attr("src", "YC02.gif");
			break;
		case -1:
			_statusImageTag.attr("src", "YC03.gif");
			break;
		case 0:
			_statusImageTag.attr("src", "YC04.gif");
			break;
		case 1:
			_statusImageTag.attr("src", "YC05.gif");
			break;
		case 2:
			_statusImageTag.attr("src", "YC06.gif");
			break;
		case 3:
			_statusImageTag.attr("src", "YC07.gif");
			break;
		}
		_statusImageAlign.appendChild(_statusImageTag);

		return _statusDom;
	}

	private Element createProductImage(Document doc, String imageSrc) {

		Element _productImageDom = doc.createElement("div");
		String _productImageDomStyle = "position:relative; width:20%; float:left; height:50px; line-height:50px; padding-top:5px";
		_productImageDom.attr("style", _productImageDomStyle);

		Element _productImageAlign = doc.createElement("a");
		_productImageAlign.attr("style", "text-align:center;");
		_productImageDom.appendChild(_productImageAlign);

		Element _productImageTag = doc.createElement("img");
		_productImageTag.attr("width", "50px");
		_productImageTag.attr("height", "50px");
		_productImageTag.attr("src", productThumnailreplacePath(imageSrc));
		// _productImageTag.attr("class", "productFold");
		// _productImageTag.attr("style", "cursor:pointer;");
		_productImageAlign.appendChild(_productImageTag);

		return _productImageDom;
	}

	private Element createProductName(Document doc, String name) {

		Element _productNameDom = doc.createElement("div");
		String _productNameDomStyle = "position:relative; width:15%; float:left; height:50px; line-height:50px; padding-top:5px; word-wrap: break-word;";
		_productNameDom.attr("style", _productNameDomStyle);
		_productNameDom.html(name);

		return _productNameDom;
	}

	private Element createProductPrice(Document doc, Integer price) {

		Element _productNameDom = doc.createElement("div");
		String _productNameDomStyle = "position:relative; width:15%; float:left; height:50px; line-height:50px; padding-top:5px";
		_productNameDom.attr("style", _productNameDomStyle);
		_productNameDom.html(price.toString());

		return _productNameDom;
	}

	private Element createProductDescriptionToggle(Document doc) {

		String _productDescriptionToggle = "DETAIL";
		Element _productDescriptionToggleDom = doc.createElement("div");
		String _productDescriptionToggleDomStyle = "position:relative; width:10%; float:left; height:50px; line-height:50px; padding-top:5px";
		_productDescriptionToggleDom.attr("style",_productDescriptionToggleDomStyle);
		_productDescriptionToggleDom.attr("class", "productFold");
		_productDescriptionToggleDom.html(_productDescriptionToggle);

		return _productDescriptionToggleDom;
	}

	private Element createProductDesciption(Document doc, String desciption) {

		Element _productDescriptionDom = doc.createElement("div");
		String _productDescriptionDomStyle = "position:relative; clear:both; width:100%; display:none;";
		_productDescriptionDom.attr("style", _productDescriptionDomStyle);
		_productDescriptionDom.html(replacePath(desciption));

		return _productDescriptionDom;
	}

	private String replacePath(String str) {
		if (org.apache.commons.exec.OS.isFamilyWindows()) {
			return str.replace("\\userfiles", PreferenceConstants.P_EVE_HOME
					+ File.separator + "userfiles");
		}
		return str.replace("/userfiles", PreferenceConstants.P_EVE_HOME
				+ File.separator + "userfiles");
	}
	
	private String productThumnailreplacePath(String str) {
		
		return str.replace("userfiles", PreferenceConstants.P_EVE_HOME
				+ File.separator + "userfiles");
	}

	private String loadTemplate() {
		String html = null;
		InputStream in = getClass().getResourceAsStream("/res/template.htm");
		try {
			byte[] b = new byte[in.available()];
			in.read(b);
			html = new String(b, StandardCharsets.UTF_8);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return html;
	}

	private String loadSubTemplate() {
		String html = null;
		InputStream in = getClass().getResourceAsStream("/res/sub.htm");
		try {
			byte[] b = new byte[in.available()];
			in.read(b);
			html = new String(b, StandardCharsets.UTF_8);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return html;
	}

	private void save(Path tmp, String html) {
		tmp = DataManager.getInstance().getTempPath();
		OutputStream outputStream = null;
		try {
			outputStream = new FileOutputStream(tmp.toString() + File.separator
					+ name);
			outputStream.write(html.getBytes());
			outputStream.flush();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} finally {

			if (outputStream != null) {
				try {
					outputStream.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			outputStream = null;
		}
	}

	private void batchJQuery(Path tmp) {
		InputStream in = getClass().getResourceAsStream("/res/jslib.tar.gz"); // TODO

		try {
			byte[] b = new byte[in.available()];
			for (int offset = 0, n = 0; offset < b.length; offset += n) {
				n = in.read(b, offset, b.length - offset);
			}
			DataManager.extractGZipTo(b, tmp.toFile());
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		} finally {
			if (in != null) {
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
因为在一个公司的招聘网站上，看到它上面的Android开发需要了解XML和JSON的解析，所以写了这个博客。我的想法是，在网站上找一个XML和JSON的数据，然后用Android解析一下。

##HTTP请求

写的时候，突然发现我对于HTTP的请求不知道怎么写了，然后看了别人的博客，这里做一个记录。

```
void getResponse(){
        new Thread(new Runnable() {
            @Override
            public void run() {
                HttpURLConnection urlConnection =  null;
                BufferedReader reader = null;
                try {
                    String string = "http://www.people.com.cn/rss/politics.xml";
                    URL url = new URL(string);
                    urlConnection = (HttpURLConnection)url.openConnection();
                    urlConnection.setRequestMethod("GET");
                    urlConnection.connect();
                    int responseCode = urlConnection.getResponseCode();
                    if(responseCode == HttpURLConnection.HTTP_OK){
                        InputStream inputStream = urlConnection.getInputStream();
                        parseXml(inputStream);
//                        reader = new BufferedReader(new InputStreamReader(inputStream)); //将字节流转化成字符流
//                        StringBuilder response = new StringBuilder();
//                        String line;
//                        while ((line = reader.readLine())!= null) {
//                            response.append(line).append('\n');
//                        }
                    }

                } catch (Exception e) {
                    e.printStackTrace();
                }finally {
                    if(reader != null){
                        try {
                            reader.close();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                    if(urlConnection != null){
                        urlConnection.disconnect();
                    }
                }
            }
        }).start();
    }
```

##XML解析

Android有三种用于解析XML数据的主要方式：DOM、SAX和Pull。因为DOM解析需要先将整个XML文档读到内存，而SAX和Pull解析都是基于流的，所以，在Android开发中，不推荐使用DOM的解析方式。其中，Android系统内部默认使用Pull来解析XML文件。

###SAX解析

当使用SAX解析器进行操作时会触发一系列的事件。当扫描到文档（Document）开始与结束、元素（Element）开始与结束时都会调用相关的处理方法，并由这些操作方法作出相应的操作，直到整个文档扫描结束。

下面给出一个例子，这里XML的文件使用的是人民网上的RSS新闻，[http://www.people.com.cn/rss/politics.xml](http://www.people.com.cn/rss/politics.xml)，这里主要是提取每个新闻的题目。

```
void parseXml(InputStream inputStream){
    try{
        SAXParserFactory factory = SAXParserFactory.newInstance();
        SAXParser parser = factory.newSAXParser();
        parser.parse(inputStream,new DefaultHandler(){
            StringBuilder stringBuilder;
            boolean inItem;
            @Override
            public void startElement(String uri, String localName, String qName, Attributes attributes) throws SAXException {
                if("item".equals(localName)){
                    inItem = true;
                }else if("title".equals(localName) && inItem == true){
                    stringBuilder = new StringBuilder();
                }
            }

            @Override
            public void endElement(String uri, String localName, String qName) throws SAXException {
                if("item".equals(localName)){
                    inItem = false;
                }else if("title".equals(localName) && inItem == true){
                    Log.e("TAG",stringBuilder.toString());
                }
            }

            @Override
            public void characters(char[] ch, int start, int length) throws SAXException {
                if(stringBuilder != null){
                    for(int i=start; i < start + length;i++){
                        stringBuilder.append(ch[i]);
                    }
                }
            }
        });
    }catch (Exception e){
        e.printStackTrace();
    }
}
```
这里写得非常简单，需要注意的只有一点，就是我们要解析的title是在item里面的，和item同一级的，还有title标签，所以，我这里做了一点处理，保证解析到的title都在item里面。

###Pull解析

Pull解析和SAX类似，也是从头到尾扫描，我们可以在程序中关注我们感兴趣的标签，然后提取出来。代码和上面的代码类似。

```
void parseXml(InputStream inputStream){
    try {
        XmlPullParser parser = Xml.newPullParser();
        parser.setInput(inputStream,"utf-8");
        int type = parser.getEventType();
        boolean inItem = false;
        while(type != XmlPullParser.END_DOCUMENT){
            switch (type){
                case XmlPullParser.START_TAG:
                    if("item".equals(parser.getName())){
                        inItem = true;
                    }
                    if("title".equals(parser.getName()) && inItem == true){
                        Log.e("TAG",parser.nextText());
                    }
                    break;
                case XmlPullParser.END_TAG:
                    if("item".equals(parser.getName())){
                        inItem = false;
                    }
                    break;
                default:
                    break;
            }
            type = parser.next();
        }

    } catch (Exception e) {
        e.printStackTrace();
    }
}
```

##JSON解析

这里使用的JSON字符串，是我在书上抄的一个JSON字符串。后面两种解析方法都是解析这个JSON字符串。
```
{
	"person": {
		"name": "John",
		"age": 30,
		"children": [{
			"name": "Billy",
			"age": 5
		}, {
			"name": "Sarah",
			"age": 7
		}, {
			"name": "Tommy",
			"age": 9
		}]
	}
}
```
###使用JSONObject和JSONArray

JSON字符串，感觉就像俄罗斯套娃一样。而使用JSONObject和JSONArray的解析方式就像是把俄罗斯套娃从外层开始拆。下面是具体的代码。

```
void parseJosn(String string){
    try {
        JSONObject total = new JSONObject(string);
        JSONObject person = total.getJSONObject("person");
        Log.e("TAG",person.getString("name") + " "+ person.getInt("age") + "\n");
        JSONArray children = person.getJSONArray("children");
        for(int i=0;i<children.length();i++){
            JSONObject child = children.getJSONObject(i);
            Log.e("TAG",child.getString("name")+" " + child.getInt("age") + "\n");
        }
    } catch (JSONException e) {
        e.printStackTrace();
    }
}
```

###使用Gson
Gson是谷歌的一个开源项目，要使用Gson，需要添加依赖。
```
dependencies {
  implementation 'com.google.code.gson:gson:2.8.5'
}
```
Gson主要提供了两个函数，用于把JSON字符串转化为一个Java对象，和把一个Java对象转化为JSON字符串。
```
 /* This method deserializes the specified Json into an object of the specified class. */
public <T> T fromJson(String json, Class<T> classOfT) ;
/* This method serializes the specified object into its equivalent Json representation. */
public String toJson(Object src) ;
```
Gson的使用非常简单，但要写出对应的对象比较麻烦，这里，有一个Android Studio的插件GsonFormat 可以让我们方便的生成需要的对象。首先是安装GsonFormat 。

1.  Android studio File->Settings..->Plugins–>Browse repositores..搜索GsonFormat。 
2. 安装插件,重启android studio。

使用也非常简单。在类里面右键选择Generate， 选择GsonFormat把JSON粘贴到里面即可一键生成。
针对于上面JSON字符串，生成的代码如下，这里我省略了get和set方法，并把属性改为了public。
```
public class Person {

    public PersonBean person;

    public static class PersonBean {

        public String name;
        public int age;
        public List<ChildrenBean> children;

        public static class ChildrenBean {
            private String name;
            private int age;
        }
    }
}
```
解析只需要两行代码即可搞定。
```
void parseJosn(String string){
    Gson gson = new Gson();
    Person person= gson.fromJson(string, Person.class);
    Log.e("TAG",gson.toJson(person)); /* 检查转换的结果 */
}
```

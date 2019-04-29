#ifndef __XML_DEFINE_H__
#define __XML_DEFINE_H__

static const char *_introspect_xml_main_empty = "\
<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\
<node name=\"/\">\
  <interface name=\"org.freedesktop.DBus.Introspectable\">\
    <method name=\"Introspect\">\
      <arg name=\"xml_data\" type=\"s\" direction=\"out\"/>\
    </method>\
  </interface>\
</node>";

static const char *_introspect_xml_tmpl = "\
<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\
<node name=\"/\">\
  <interface name=\"org.freedesktop.DBus.Introspectable\">\
    <method name=\"Introspect\">\
      <arg name=\"xml_data\" type=\"s\" direction=\"out\"/>\
    </method>\
  </interface>\
  <interface name=\"org.freedesktop.DBus.Properties\">\
    <method name=\"Get\">\
      <arg name=\"interface_name\" type=\"s\" direction=\"in\"/>\
      <arg name=\"property_name\" type=\"s\" direction=\"in\"/>\
      <arg name=\"value\" type=\"v\" direction=\"out\"/>\
    </method>\
    <method name=\"Set\">\
      <arg name=\"interface_name\" type=\"s\" direction=\"in\"/>\
      <arg name=\"property_name\" type=\"s\" direction=\"in\"/>\
      <arg name=\"value\" type=\"v\" direction=\"in\"/>\
    </method>\
    <method name=\"GetAll\">\
      <arg name=\"interface_name\" type=\"s\" direction=\"in\"/>\
      <arg name=\"props\" type=\"{sv}\" direction=\"out\"/>\
    </method>\
  </interface>\
  <node name=\"HelloWorld\"/>\
  <node name=\"StatusNotifierItem\"/>\
</node>";

// DONE: all
static const char *_introspect_xml_tmpl_statusnotifier = "\
<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\
<node name=\"/\">\
  <interface name=\"org.kde.StatusNotifierItem\">\
    <property name=\"Category\" type=\"s\" access=\"read\"/>\
    <property name=\"Id\" type=\"s\" access=\"read\"/>\
    <property name=\"Title\" type=\"s\" access=\"read\"/>\
    <property name=\"Status\" type=\"s\" access=\"read\"/>\
    <property name=\"WindowId\" type=\"u\" access=\"read\"/>\
    <property name=\"IconName\" type=\"s\" access=\"read\"/>\
    <property name=\"IconPixmap\" type=\"a(iiay)\" access=\"read\"/>\
    <property name=\"OverlayIconName\" type=\"s\" access=\"read\"/>\
    <property name=\"OverlayIconPixmap\" type=\"a(iiay)\" access=\"read\"/>\
    <property name=\"AttentionIconName\" type=\"s\" access=\"read\"/>\
    <property name=\"AttentionIconPixmap\" type=\"a(iiay)\" access=\"read\"/>\
    <property name=\"AttentionMovieName\" type=\"s\" access=\"read\"/>\
    <property name=\"ToolTip\" type=\"(sa(iiay)ss)\" access=\"read\"/>\
    <method name=\"ContextMenu\">\
      <arg name=\"x\" type=\"i\" direction=\"in\"/>\
      <arg name=\"y\" type=\"i\" direction=\"in\"/>\
    </method>\
    <method name=\"Activate\">\
      <arg name=\"x\" type=\"i\" direction=\"in\"/>\
      <arg name=\"y\" type=\"i\" direction=\"in\"/>\
    </method>\
    <method name=\"SecondaryActivate\">\
      <arg name=\"x\" type=\"i\" direction=\"in\"/>\
      <arg name=\"y\" type=\"i\" direction=\"in\"/>\
    </method>\
    <method name=\"Scroll\">\
      <arg name=\"delta\" type=\"i\" direction=\"in\"/>\
      <arg name=\"orientation\" type=\"s\" direction=\"in\"/>\
    </method>\
    <signal name=\"NewTitle\"></signal>\
    <signal name=\"NewIcon\"></signal>\
    <signal name=\"NewAttentionIcon\"></signal>\
    <signal name=\"NewOverlayIcon\"></signal>\
    <signal name=\"NewToolTip\"></signal>\
    <signal name=\"NewStatus\">\
      <arg name=\"status\" type=\"s\"/>\
    </signal>\
  </interface>\
</node>";

// sean: all
static const char *_introspect_xml_hello = "\
<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\">\
<node name=\"/\">\
    <interface name=\"org.freedesktop.DBus.Introspectable\">\
    <method name=\"Introspect\">\
        <arg name=\"xml_data\" type=\"s\" direction=\"out\"/>\
    </method>\
    </interface>\
    <interface name=\"com.kssim.test.bus\">\
        <method name=\"HelloWorldMethod\">\
            <arg name=\"str\" type=\"s\" direction=\"in\"/>\
            <arg name=\"ret\" type=\"s\" direction=\"out\"/>\
        </method>\
        <signal name=\"HelloWorldSignal\">\
            <arg name=\"mes\" type=\"s\"/>\
        </signal>\
    </interface>\
</node>";

#endif //__XML_DEFINE_H__
// 8 august 2014
#include "gtkcontrolspy.h"

GHashTable *widgets;

// cheating (but I'm not the only one; see GdkEvent!)
typedef union GIInfo GIInfo;
union GIInfo {
	GIBaseInfo *base;
	GIArgInfo *arg;
	GICallableInfo *call;
	GIFunctionInfo *func;
	GISignalInfo *sig;
	GIVFuncInfo *vfunc;
	GIConstantInfo *constant;
	GIFieldInfo *field;
	GIPropertyInfo *prop;
	GIRegisteredTypeInfo *reg;
	GIEnumInfo *en;
	GIInterfaceInfo *iface;
	GIObjectInfo *obj;
	GIStructInfo *st;
	GIUnionInfo *un;
	GITypeInfo *type;
};

static inline gboolean isWidget(GIInfo info)
{
	GType gtype;

	if (g_base_info_get_type(info.base) != GI_INFO_TYPE_OBJECT)
		return FALSE;
	gtype = g_registered_type_info_get_g_type(info.reg);
	if (!g_type_is_a(gtype, GTK_TYPE_WIDGET))
		return FALSE;
	return TRUE;
}

static void addProperty(GIInfo info, gint n, Property *props)
{
	GIInfo p;

	p.prop = g_object_info_get_property(info.obj, n);
	props[n].Name = g_strdup(g_base_info_get_name(p.base));
	props[n].Valid = getBinding(p.prop, &props[n].Editor, &props[n].BindTo);
if(!props[n].Valid)
printf("skipping unknown/illegal prop %s::%s (%s)\n", g_object_info_get_type_name(info.obj), props[n].Name, g_type_tag_to_string(g_type_info_get_tag(g_property_info_get_type(p.prop))));
}

char *collectWidgets(char *repo, char *version)
{
	GError *err = NULL;
	gint i, n;

	if (g_irepository_require(NULL, repo, version, 0, &err) == NULL)
		return g_strdup(err->message);
	widgets = g_hash_table_new(g_str_hash, g_str_equal);
	n = g_irepository_get_n_infos(NULL, repo);
	for (i = 0; i < n; i++) {
		GIInfo info, parent;
		Widget *widget;
		gint ip, np;

		info.base = g_irepository_get_info(NULL, repo, i);
		if (!isWidget(info))
			continue;
		widget = g_new0(Widget, 1);
		widget->Name = g_strdup(g_object_info_get_type_name(info.obj));
		parent.obj = g_object_info_get_parent(info.obj);
		// don't worry about going past GtkWidget; the map lookup for the Properties notebook will return NULL and stop when we reach that point
		widget->Derived = g_strdup(g_object_info_get_type_name(parent.obj));
		np = g_object_info_get_n_properties(info.obj);
		widget->nProperties = np;
		widget->Properties = g_new0(Property, widget->nProperties);
		widget->GType = g_registered_type_info_get_g_type(info.reg);
		widget->Instantiable = !g_object_info_get_abstract(info.obj);
		for (ip = 0; ip < np; ip++)
			addProperty(info, ip, widget->Properties);
		g_hash_table_insert(widgets, widget->Name, widget);
	}
	return NULL;
}

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
	GIInfo p, ptype, pi;

	p.prop = g_object_info_get_property(info.obj, n);
	props[n].Name = g_strdup(g_base_info_get_name(p.base));
	ptype.type = g_property_info_get_type(p.prop);
	props[n].TypeTag = g_type_info_get_tag(ptype.type);
	switch (props[n].TypeTag) {
	case GI_TYPE_TAG_INTERFACE:
		pi.base = g_type_info_get_interface(ptype.type);
		props[n].TypeName = g_strdup(g_registered_type_info_get_type_name(pi.reg));
		props[n].TypeType = g_base_info_get_type(pi.base);
		break;
	case GI_TYPE_TAG_VOID:
	case GI_TYPE_TAG_ARRAY:
	case GI_TYPE_TAG_GLIST:
	case GI_TYPE_TAG_GSLIST:
	case GI_TYPE_TAG_GHASH:
	case GI_TYPE_TAG_ERROR:
printf("skipping UNKNOWN prop %s::%s (%s)\n", g_object_info_get_type_name(info.obj), props[n].Name, g_type_tag_to_string(props[n].TypeTag));
		props[n].Valid = FALSE;
		return;
	}
	props[n].Valid = TRUE;
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
		widget->Derived = g_strdup(g_object_info_get_type_name(parent.obj));
		np = g_object_info_get_n_properties(info.obj);
		widget->nProperties = np;
		widget->Properties = g_new0(Property, widget->nProperties);
		for (ip = 0; ip < np; ip++)
			addProperty(info, ip, widget->Properties);
		g_hash_table_insert(widgets, widget->Name, widget);
	}
	return NULL;
}

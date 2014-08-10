// 9 august 2014
#include "gtkcontrolspy.h"

static void fillEnumType(GtkComboBoxText *cb, GIEnumInfo *et)
{
	gint i, n;

	n = g_enum_info_get_n_values(et);
	for (i = 0; i < n; i++) {
		GIBaseInfo *value;

		value = (GIBaseInfo *) g_enum_info_get_value(et, i);
		gtk_combo_box_text_append(cb, NULL, g_base_info_get_name(value));
	}
}

static gboolean doRegistered(GIRegisteredTypeInfo *ri, GtkWidget **widget, char **bindto)
{
	GType type;

	type = g_registered_type_info_get_g_type(ri);
	if (g_type_is_a(type, GDK_TYPE_RGBA)) {
		*widget = gtk_color_button_new();
		*bindto = "rgba";
		return TRUE;
	}
	if (g_type_is_a(type, GDK_TYPE_COLOR)) {
		*widget = gtk_color_button_new();
		*bindto = "color";
		return TRUE;
	}
	return FALSE;
}

gboolean getBinding(GIPropertyInfo *pi, GtkWidget **widget, char **bindto)
{
	GITypeInfo *type;
	gboolean ispointer;
	gdouble min = (gdouble) G_MININT;
	gdouble max = (gdouble) G_MAXINT;
	GIBaseInfo *iface;

	type = g_property_info_get_type(pi);
	ispointer = g_type_info_is_pointer(type);
	switch (g_type_info_get_tag(type)) {
	case GI_TYPE_TAG_INTERFACE:
		if (ispointer)
			break;
		iface = g_type_info_get_interface(type);
		switch (g_base_info_get_type(iface)) {
		case GI_INFO_TYPE_ENUM:
			*widget = gtk_combo_box_text_new();
			fillEnumType(GTK_COMBO_BOX_TEXT(*widget), (GIEnumInfo *) iface);
			*bindto = "active";
			return TRUE;
		case GI_INFO_TYPE_FLAGS:
			// TODO
			break;
		case GI_INFO_TYPE_OBJECT:
		case GI_INFO_TYPE_INTERFACE:
		case GI_INFO_TYPE_STRUCT:
		case GI_INFO_TYPE_UNION:
			return doRegistered((GIRegisteredTypeInfo *) iface, widget, bindto);
		}
		break;
	case GI_TYPE_TAG_ARRAY:
	case GI_TYPE_TAG_GLIST:
	case GI_TYPE_TAG_GSLIST:
	case GI_TYPE_TAG_GHASH:
	case GI_TYPE_TAG_ERROR:
	case GI_TYPE_TAG_VOID:
		// TODO
		break;
	case GI_TYPE_TAG_UTF8:
		if (ispointer) {
			*widget = gtk_entry_new();
			*bindto = "text";
			return TRUE;
		}
		break;
	case GI_TYPE_TAG_UINT8:
	case GI_TYPE_TAG_UINT16:
	case GI_TYPE_TAG_UINT32:
	case GI_TYPE_TAG_UINT64:
		min = 0;
		max = (gdouble) G_MAXUINT;
		// fall through
	case GI_TYPE_TAG_INT8:
	case GI_TYPE_TAG_INT16:
	case GI_TYPE_TAG_INT32:
	case GI_TYPE_TAG_INT64:
		if (!ispointer) {
			*widget = gtk_spin_button_new(NULL, 1.0, 0);
			gtk_spin_button_set_range(GTK_SPIN_BUTTON(*widget), min, max);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(*widget), 0);
			*bindto = "value";
			return TRUE;
		}
		break;
	case GI_TYPE_TAG_FLOAT:
	case GI_TYPE_TAG_DOUBLE:
		if (!ispointer) {
			// reasonable default for now
			*widget = gtk_spin_button_new(NULL, 0.1, 1);
			gtk_spin_button_set_range(GTK_SPIN_BUTTON(*widget), -G_MAXDOUBLE, G_MAXDOUBLE);
			gtk_spin_button_set_value(GTK_SPIN_BUTTON(*widget), 0);
			*bindto = "value";
			return TRUE;
		}
		break;
	case GI_TYPE_TAG_BOOLEAN:
		if (!ispointer) {
			*widget = gtk_check_button_new();
			*bindto = "active";
			return TRUE;
		}
		break;
	}
	return FALSE;
}

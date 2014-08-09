// 9 august 2014
#include "gtkcontrolspy.h"

gboolean getBinding(GIPropertyInfo *pi, GtkWidget **widget, char **bindto)
{
	GITypeInfo *type;
	gboolean ispointer;
	gdouble min = (gdouble) G_MININT;
	gdouble max = (gdouble) G_MAXINT;

	type = g_property_info_get_type(pi);
	ispointer = g_type_info_is_pointer(type);
	switch (g_type_info_get_tag(type)) {
	/* TODO
	case GI_INFO_TYPE_ENUM:
	case GI_INFO_TYPE_FLAGS:
	case GI_INFO_TYPE_OBJECT:
	case GI_INFO_TYPE_INTERFACE:
	case GI_INFO_TYPE_STRUCT:
	case GI_INFO_TYPE_UNION:
	*/
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

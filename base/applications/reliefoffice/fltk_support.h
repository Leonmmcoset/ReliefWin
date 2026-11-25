#ifndef RELIEFOS_FLTK_SUPPORT_H
#define RELIEFOS_FLTK_SUPPORT_H

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Widget.H>
#include <FL/Fl_Window.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>
#include <libintl.h>
#include <locale.h>
#include <windows.h>



#ifndef T
#define T(text) gettext(text)
#endif

namespace reliefos_fltk {

inline void translate_dialogs()
{
    fl_ok = T("OK");
    fl_cancel = T("Cancel");
    fl_close = T("Close");
    fl_yes = T("Yes");
    fl_no = T("No");
    Fl_File_Chooser::add_favorites_label = T("Add to Favorites");
    Fl_File_Chooser::all_files_label = T("All Files (*)");
    Fl_File_Chooser::custom_filter_label = T("Custom Filter");
    Fl_File_Chooser::existing_file_label = T("Please choose an existing file!");
    Fl_File_Chooser::favorites_label = T("Favorites");
    Fl_File_Chooser::filename_label = T("File name:");
    Fl_File_Chooser::filesystems_label = T("File Systems");
    Fl_File_Chooser::manage_favorites_label = T("Manage Favorites");
    Fl_File_Chooser::new_directory_label = T("New Directory?");
    Fl_File_Chooser::new_directory_tooltip = T("Create a new directory");
    Fl_File_Chooser::preview_label = T("Preview");
    Fl_File_Chooser::save_label = T("Save");
    Fl_File_Chooser::show_label = T("Show:");
    Fl_File_Chooser::hidden_label = T("Show hidden files");
}

inline void init(const char *app)
{
    setlocale(LC_ALL, "");
    reliefoffice_init_locale(app);
    Fl::set_font(FL_HELVETICA, "Noto Sans SC");
    Fl::set_font(FL_HELVETICA_BOLD, "BNoto Sans SC");
    Fl::set_font(FL_HELVETICA_ITALIC, "INoto Sans SC");
    Fl::set_font(FL_HELVETICA_BOLD_ITALIC, "PNoto Sans SC");
    Fl::set_font(FL_COURIER, "Noto Sans SC");
    Fl::set_font(FL_COURIER_BOLD, "BNoto Sans SC");
    Fl::set_font(FL_COURIER_ITALIC, "INoto Sans SC");
    Fl::set_font(FL_COURIER_BOLD_ITALIC, "PNoto Sans SC");
    Fl::set_font(FL_TIMES, "Noto Sans SC");
    Fl::set_font(FL_TIMES_BOLD, "BNoto Sans SC");
    Fl::set_font(FL_TIMES_ITALIC, "INoto Sans SC");
    Fl::set_font(FL_TIMES_BOLD_ITALIC, "PNoto Sans SC");
    fl_message_font(FL_HELVETICA, 15);
    translate_dialogs();
    Fl::scheme("gtk+");
    HICON icon = LoadIconW(GetModuleHandleW(nullptr), MAKEINTRESOURCEW(101));
    Fl_Window::default_icons(icon, icon);
}

inline void style(Fl_Widget *widget)
{
    if (!widget) return;
    // All standard faces already map to CJK fonts. Keep explicit heading
    // weights/sizes, while making ordinary widget labels slightly larger.
    if (widget->labelsize() == FL_NORMAL_SIZE) widget->labelsize(15);
    Fl_Group *group = widget->as_group();
    if (group) {
        for (int index = 0; index < group->children(); ++index)
            style(group->child(index));
    }
}

/* Preserve filename/path bytes; FLTK provides UTF-8 editing and selection.
 * A byte-bounded backend must reject overlong input rather than truncating
 * the final multibyte character or silently changing the selected path. */
inline bool copy_input(char *destination, unsigned capacity, const char *text)
{
    if (!destination || !text || !capacity) return false;
    unsigned length = 0;
    while (text[length]) {
        if (length + 1 >= capacity) return false;
        ++length;
    }
    for (unsigned index = 0; index <= length; ++index)
        destination[index] = text[index];
    return true;
}

} // namespace reliefos_fltk
#endif

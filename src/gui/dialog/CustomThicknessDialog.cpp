#include "CustomThicknessDialog.h"

CustomThicknessDialog::CustomThicknessDialog(GladeSearchpath* gladeSearchPath, int thickness):
        GladeGui(gladeSearchPath, "customThickness.glade", "customThicknessDialog") {
    GtkWidget* scaleThickness = get("scaleThickness");

    gtk_range_set_value(GTK_RANGE(scaleThickness), static_cast<int>(thickness / 255.0 * 100));

    setPreviewImage(thickness);

    g_signal_connect(scaleThickness, "change-value",
                     G_CALLBACK(+[](GtkRange* range, GtkScrollType scroll, gdouble value, CustomThicknessDialog* self) {
                         self->setPreviewImage((int)(value / 100 * 255));
                         gtk_range_set_value(range, value);
                     }),
                     this);
}

CustomThicknessDialog::~CustomThicknessDialog() = default;

const int PREVIEW_WIDTH = 70;
const int PREVIEW_HEIGTH = 50;
const int PREVIEW_BORDER = 10;

void CustomThicknessDialog::setPreviewImage(int thickness) {
    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, PREVIEW_WIDTH, PREVIEW_HEIGTH);
    cairo_t* cr = cairo_create(surface);

    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgb(cr, 255, 255, 255);
    cairo_rectangle(cr, 0, 0, PREVIEW_WIDTH, PREVIEW_WIDTH);
    cairo_fill(cr);

    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
    cairo_set_source_rgba(cr, 0, 0x80 / 255.0, 0, thickness / 255.0);
    cairo_rectangle(cr, PREVIEW_BORDER, PREVIEW_BORDER, PREVIEW_WIDTH - PREVIEW_BORDER * 2,
                    PREVIEW_HEIGTH - PREVIEW_BORDER * 2);
    cairo_fill(cr);

    cairo_set_line_width(cr, 5);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_rgb(cr, 0, 0x80 / 255.0, 0);
    cairo_rectangle(cr, PREVIEW_BORDER, PREVIEW_BORDER, PREVIEW_WIDTH - PREVIEW_BORDER * 2,
                    PREVIEW_HEIGTH - PREVIEW_BORDER * 2);
    cairo_stroke(cr);

    cairo_destroy(cr);

    GtkWidget* preview = get("imgPreview");
    gtk_image_set_from_surface(GTK_IMAGE(preview), surface);
}

auto CustomThicknessDialog::getResultThickness() const -> double { return static_cast<double>(resultThickness); }

void CustomThicknessDialog::show(GtkWindow* parent) {
    gtk_window_set_transient_for(GTK_WINDOW(this->window), parent);
    int result = gtk_dialog_run(GTK_DIALOG(this->window));
    gtk_widget_hide(this->window);

    // OK Button
    if (result == 1) {

        GtkWidget* scaleThickness = get("scaleThickness");
        resultThickness = static_cast<int>(gtk_range_get_value(GTK_RANGE(scaleThickness)) * 255.0 / 100.0);
    } else {
        resultThickness = -1;
    }
}

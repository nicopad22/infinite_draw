#include <gtk/gtk.h>
#include <math.h>
#include "infinite_draw.h"
#define zoom_increase 0.09

//surface for drawing.
static cairo_surface_t *surface = NULL;
//to control if the user is dragging across.
static gboolean is_dragging = FALSE;
//last position; used to control how much the screen is moved.
static vector2_t last_pos;
//offset for being able to move things around
static vector2_t offset;
//zoom level
static double zoom = 1;
//size of the window
static pixel_vector_t window_size;
//brush size
static float brush_size = 2;
//brush color
static pixel_t brush_color;



int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
    return status;
}

/* clear the screen. */
void clear_screen(void) {
    cairo_t *cr;

    cr = cairo_create(surface);

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    cairo_destroy(cr);
}

/* create new surface to store drawings, with appropriate size. */
gboolean configure_event_cb(GtkWidget *widget, GdkEventConfigure *event, gpointer data) {
    if (surface)
        cairo_surface_destroy(surface);

    surface = gdk_window_create_similar_surface(gtk_widget_get_window(widget),
                                                CAIRO_CONTENT_COLOR,
                                                gtk_widget_get_allocated_width(widget),
                                                gtk_widget_get_allocated_height(widget));

    /* Initialize the surface to white */
    clear_screen();

    /* We've handled the configure event, no need for further processing. */
    return TRUE;
}

/* callback for when our draw area changes size, to update the local size we have. */
void get_window_size_on_change(GtkWidget *widget, GtkAllocation *allocation, void *data) {
    window_size.x = allocation->width;
    window_size.y = allocation->height;
}

/* draw surface to the screen */
gboolean draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data) {
    cairo_set_source_surface(cr, surface, 0, 0);

    //Paint scribbles
    paint_window(cr);

    return FALSE;
}

/* paint stored lines into the surface */
void paint_window(cairo_t *cr) {
    vector2_node_t *cursor = head;
    
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    float last_brush_size;

    while (cursor) {
        pixel_vector_t screenPos = vpos_to_pixel(cursor->vector);
        switch(cursor->state) {
            case start_and_end: break;

            case start:
                last_brush_size = cursor->brush_size / zoom;
                cairo_set_source_rgb(cr, cursor->color.red, cursor->color.green, cursor->color.blue);
                cairo_set_line_width(cr, last_brush_size);
                cairo_move_to(cr, screenPos.x, screenPos.y);
                break;

            case path:
                cairo_line_to(cr, screenPos.x, screenPos.y);
                cairo_move_to(cr, screenPos.x, screenPos.y);
                break;

            case end:
                cairo_line_to(cr, screenPos.x, screenPos.y);
                break;
                
        }
        cursor = cursor->next;
    }
    cairo_stroke(cr);
}

/* called when mouse is clicked; does not get called each frame, but rather once when you press the mouse. */
gboolean dw_clicked(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    //in case something went horribly wrong
    if (surface == NULL)
        return FALSE;

    if (event->button == GDK_BUTTON_PRIMARY) {
        vector2_node_t *StartNode = malloc(sizeof(vector2_node_t));
        //set positions and initialize .next
        StartNode->vector = pixel_to_vpos((pixel_vector_t){event->x, event->y});
        StartNode->color = brush_color;
        StartNode->brush_size = brush_size * zoom;
        StartNode->next = NULL;

        //if this is the first click: point both head & last to the new node.
        //plus, set the state to start & end, as we dont have an end node yet.
        if (head == NULL) {
            head = StartNode;
            last = StartNode;
            StartNode->state = start_and_end;
        //else: the list is not empty, so we append to the end, and point currently last node to new node.
        } else {
            last->next = StartNode;
            last = StartNode;
            StartNode->state = start_and_end;
        }   
    }
        
    gtk_widget_queue_draw(widget);

    last_pos.x = event->x;
    last_pos.y = event->y;

    //everything has gone well. (hopefully)
    return TRUE;
}

/* gets called when we already clicked the mouse, but we moved to a different position. */
gboolean dw_moved(GtkWidget *widget, GdkEventMotion *event, gpointer data) {
    //in case something goes wrong
    if (surface == NULL)
        return FALSE;

    if (event->state & GDK_BUTTON1_MASK) {
        vector2_node_t *NewNode = malloc(sizeof(vector2_node_t));

        NewNode->vector = pixel_to_vpos((pixel_vector_t){event->x, event->y});
        NewNode->next = NULL;

        //whatever happens, we definetly want the last node to point at us.
        last->next = NewNode;

        //if the state of the last node is start_and_end, then change its state to start.
        //(in other words if this stroke only has the initial click)
        if (last->state == start_and_end)
            last->state = start;
        //We already have more than one node in this stroke, so we can continue it by updating states for last node
        //and append a new node at the end.
        else if (last->state == end) {
            last->state = path;
            NewNode->state = end;
        } else  {
            //something has gone horribly wrong when setting the states up, quickly exit and report the user
            fprintf(stderr, "ERROR: linked list not set up correctly, node had invalid state: %d\n", last->state);
            return FALSE;
        }

        last = NewNode;
        NewNode->state = end;
    }

    gtk_widget_queue_draw(widget);

    vector2_t movement = {0, 0};
    movement.x = event->x - last_pos.x;
    movement.y = event->y - last_pos.y;
    last_pos.x = event->x;
    last_pos.y = event->y;

    //movement = pixel_to_vpos((pixel_vector_t){movement.x, movement.y});

    if (is_dragging) {
        offset.x += movement.x * zoom;
        offset.y += movement.y * zoom;
    }

    //everything went well
    return TRUE;
}

/* activates when the mouse wheel gets scrolled up. */
gboolean dw_mousewheel(GtkWidget *widget, GdkEventScroll *event, gpointer data) {
    double to_increase = ((event->direction == GDK_SCROLL_UP) * -zoom_increase * zoom) +
                         ((event->direction == GDK_SCROLL_DOWN) * zoom_increase * zoom);
    zoom += to_increase;

    double x_offset_change = (to_increase * window_size.x) * (1 / (event->x / 4));
    //g_print("changed x offset by: %f\n", x_offset_change);
    double y_offset_change = (to_increase * window_size.y) * (1 / (event->y / 4));
    //g_print("changed y offset by: %f\n", y_offset_change);

    //offset.x += x_offset_change;
    //offset.y += y_offset_change;
    gtk_widget_queue_draw(widget);

    return TRUE;
}

/* on keypresses, will trigger whenever we press or release a key */
gboolean dw_keyPressed(GtkWidget *widget, GdkEventKey *event, gpointer data) {

    if (surface == NULL)
        return FALSE;

    //we want to be dragging while we hold space; and as this triggers when we press and release,
    //this boolean will be true when we are holding it.
    if (event->keyval == GDK_KEY_space) {
        is_dragging = !is_dragging;
    }

    return TRUE;
}

/* convert a current pixel location to a virtual position. */
vector2_t pixel_to_vpos(pixel_vector_t pixel) {
    return (vector2_t){(pixel.x - offset.x * zoom) * zoom, (pixel.y - offset.y * zoom) * zoom};
}

/* convert a virtual location to a current pixel location */
pixel_vector_t vpos_to_pixel(vector2_t vpos) {
    return (pixel_vector_t){round(vpos.x / zoom) + offset.x / zoom, round(vpos.y / zoom) + offset.y / zoom};
}

/* make sure to destroy cairo surface when we close the window */
void close_window(void) {
    if (surface)
        cairo_surface_destroy(surface);
    unload();
}

/* free linked list nodes */
void unload(void) {
    vector2_node_t *cursor = head;
    while (cursor) {
        vector2_node_t *temp = cursor;
        cursor = cursor->next;
        free(temp);
    }
    head = last = NULL;

}

/* activate window & set up widgets, connections, etc */
void activate(GtkApplication *app, gpointer user_data) {
    //simple elements: a window, a frame, and the drawing area for the strokes.
    GtkWidget *window;
    GtkWidget *frame;
    GtkWidget *drawing_area;

    //create new window, and set its title.
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Drawing Area");

    //connect signal for when we close the window, the cairo surface gets destroyed as well.
    g_signal_connect(window, "destroy", G_CALLBACK(close_window), NULL);

    //set border width for the window. (to leave some space between draw area and window)
    gtk_container_set_border_width(GTK_CONTAINER(window), 8);

    //create new frame, no label. (NULL)
    frame = gtk_frame_new(NULL);
    //set the shadow for the frame
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
    //add frame to window.
    gtk_container_add(GTK_CONTAINER(window), frame);
    
    //create new draw area
    drawing_area = gtk_drawing_area_new();
    //set it so we can focus it
    gtk_widget_set_can_focus(drawing_area, TRUE);
    //set minimum size
    gtk_widget_set_size_request(drawing_area, 200, 200);
    //add draw area to the frame
    gtk_container_add(GTK_CONTAINER(frame), drawing_area);

    //Connect signal to draw the surface to the screen
    g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_cb), NULL);
    //Connect signal to create a new surface with the appropriate size
    g_signal_connect(drawing_area, "configure-event", G_CALLBACK(configure_event_cb), NULL);
    //Connect signal to update local readings of window size.
    g_signal_connect(drawing_area, "size_allocate", G_CALLBACK(get_window_size_on_change), NULL);

    //connect signals for mouse & keyboard events
    g_signal_connect(drawing_area, "motion-notify-event", G_CALLBACK(dw_moved), NULL);
    g_signal_connect(drawing_area, "button-press-event", G_CALLBACK(dw_clicked), NULL);
    g_signal_connect(drawing_area, "key-press-event", G_CALLBACK(dw_keyPressed), NULL);
    g_signal_connect(drawing_area, "scroll-event", G_CALLBACK(dw_mousewheel), NULL);


    //set events
    gtk_widget_set_events(drawing_area, gtk_widget_get_events(drawing_area) | GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK | GDK_SCROLL_MASK);

    //show widgets; otherwise nothing will happen
    gtk_widget_show_all(window);
}

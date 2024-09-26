#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

struct TodoItem {
    char task[100];
    struct TodoItem* next;
};

struct TodoItem* start = NULL;

void on_add_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *entry = GTK_WIDGET(user_data);
    const char *task = gtk_entry_get_text(GTK_ENTRY(entry));

    if (strlen(task) > 0) {
        struct TodoItem* newItem = (struct TodoItem*)malloc(sizeof(struct TodoItem));
        if (newItem != NULL) {
            strcpy(newItem->task, task);
            newItem->next = NULL;

            if (start == NULL) {
                start = newItem;
            } else {
                struct TodoItem* current = start;
                while (current->next != NULL) {
                    current = current->next;
                }
                current->next = newItem;
            }
            gtk_entry_set_text(GTK_ENTRY(entry), "");
        }
    }
}

void on_remove_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *list = GTK_WIDGET(user_data);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list));
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        int index;
        gtk_tree_model_get(model, &iter, 0, &index, -1);
        removeTodo(&start, index + 1);
        gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    }
}

void displayTodoList(GtkListStore *store) {
    GtkTreeIter iter;
    struct TodoItem* current = start;
    int count = 1;

    gtk_list_store_clear(store);

    while (current != NULL) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, count, 1, current->task, -1);
        current = current->next;
        count++;
    }
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *list;
    GtkWidget *entry;
    GtkWidget *add_button;
    GtkWidget *remove_button;
    GtkListStore *store;
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Todo List Application");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    list = gtk_tree_view_new();
    store = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);

    column = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(column, "Task");
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, renderer, TRUE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", 1);

    entry = gtk_entry_new();
    add_button = gtk_button_new_with_label("Add Task");
    remove_button = gtk_button_new_with_label("Remove Task");

    gtk_widget_set_hexpand(entry, TRUE);
    gtk_widget_set_hexpand(add_button, TRUE);
    gtk_widget_set_hexpand(remove_button, TRUE);

    g_signal_connect(G_OBJECT(add_button), "clicked", G_CALLBACK(on_add_button_clicked), entry);
    g_signal_connect(G_OBJECT(remove_button), "clicked", G_CALLBACK(on_remove_button_clicked), list);

    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    gtk_container_add(GTK_CONTAINER(window), list);
    gtk_container_add(GTK_CONTAINER(window), entry);
    gtk_container_add(GTK_CONTAINER(window), add_button);
    gtk_container_add(GTK_CONTAINER(window), remove_button);

    displayTodoList(store);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

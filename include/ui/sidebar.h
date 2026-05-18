#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <gtk/gtk.h>
#include "rules/rules.h"

GtkWidget *create_sidebar(void);

void sidebar_set_summary(GtkWidget *sidebar, const char *text);

void sidebar_set_rules_file(GtkWidget *sidebar, const char *filename);

GtkListBox *sidebar_get_rules_list(GtkWidget *sidebar);

void sidebar_bind_ruleset(GtkWidget *sidebar, RuleSet *ruleset);

void sidebar_refresh_ruleset(GtkWidget *sidebar, RuleSet *ruleset, const char *text);

#endif

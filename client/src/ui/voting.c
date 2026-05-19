#include <newt.h>
#include <string.h>

#include "voting.h"

void show_voting_menu(
    char *candidate
)
{
    newtComponent form;

    newtComponent listbox;

    newtComponent btn_vote;

    newtCenteredWindow(
        50,
        18,
        "Escolha seu candidato"
    );

    listbox =
        newtListbox(
            5,
            2,
            5,
            NEWT_FLAG_RETURNEXIT
        );

    newtListboxAppendEntry(
        listbox,
        "10 - Candidato A",
        "candidatoA"
    );

    newtListboxAppendEntry(
        listbox,
        "20 - Candidato B",
        "candidatoB"
    );

    newtListboxAppendEntry(
        listbox,
        "30 - Candidato C",
        "candidatoC"
    );

    btn_vote =
        newtButton(
            18,
            12,
            "Confirmar Voto"
        );

    form = newtForm(NULL, NULL, 0);

    newtFormAddComponents(
        form,
        listbox,
        btn_vote,
        NULL
    );

    newtRunForm(form);

    char *selected =
        (char*)newtListboxGetCurrent(listbox);

    strcpy(candidate, selected);

    newtFormDestroy(form);
}

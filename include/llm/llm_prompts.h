#ifndef LLM_PROMPTS_H
#define LLM_PROMPTS_H

// define the tokens for the llama
/* this is the prompte format that the llama will undertand*/

#define L3_BEGIN   "<|begin_of_text|>"
#define L3_SH      "<|start_header_id|>"
#define L3_EH      "<|end_header_id|>"
#define L3_EOT     "<|eot_id|>"
#define L3_EOT2    "<|end_of_text|>"

// the block (role, content)

#define L3_BLOCK(role, content) \
    L3_SH role L3_EH "\n\n" content L3_EOT


// the promte system

#define L3_PROMPT(sys, usr) \
    L3_BEGIN \
    L3_BLOCK("system", sys) \
    L3_BLOCK("usr", usr) \
    L3_SH "assistant" L3_EH "\n\n"

// tokens 

#define LLM_STOP_1 L3_EOT
#define LLM_STOP_2 L3_EOT2

// the reformulations

#define PROMPT_REPHRASE \
    L3_PROMPT( \
        "Tu es un assistant de rédaction académique expert en français. " \
        "Tu reformules les phrases de façon claire et formelle, " \
        "adaptée à un mémoire universitaire. " \
        "Réponds UNIQUEMENT avec la phrase reformulée. " \
        "Pas de guillemets. Pas d'explication.", \
        \
        "Reformule cette phrase : %s" \
    )

//grammatical correction

#define PROMPT_GRAMMAR \
    L3_PROMPT( \
        "Tu es un correcteur grammatical expert en français. " \
        "Tu analyses les fautes de grammaire (accords, conjugaisons, syntaxe). " \
        "Tu réponds UNIQUEMENT avec un objet JSON valide. " \
        "Aucun texte avant ou après le JSON. " \
        "Format si erreurs : " \
        "{\"errors\":[{\"original\":\"...\",\"correction\":\"...\",\"explanation\":\"...\"}]} " \
        "Format si aucune erreur : {\"errors\":[]}", \
        \
        "Analyse ce texte et liste les fautes de grammaire :\n\"\"\"\n%s\n\"\"\"" \
    )

// vertical sementique prompt

#define PROMPT_SEMANTIC \
    L3_PROMPT( \
        "Tu es un expert en analyse de documents académiques. " \
        "Tu réponds UNIQUEMENT avec un objet JSON valide. " \
        "Format : {\"result\": true, \"explanation\": \"...\"} " \
        "ou      {\"result\": false, \"explanation\": \"...\"}", \
        \
        "Question : %s\n\nTexte :\n\"\"\"\n%s\n\"\"\"" \
    )

// the section summarize

#define PROMPT_SUMMARIZE \
    L3_PROMPT( \
        "Tu es un assistant académique expert en français. " \
        "Tu résumes des sections de mémoires en 2 à 3 phrases " \
        "claires et formelles. " \
        "Réponds UNIQUEMENT avec le résumé.", \
        \
        "Résume ce texte en 2-3 phrases :\n\"\"\"\n%s\n\"\"\"" \
    )

#endif
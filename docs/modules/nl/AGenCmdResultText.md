### Description

This agent get result text template for a specified command and run [AGenText](AGenText.md) agent
to generate output text.

**Command class:** `command_generate_cmd_result_text`

**Parameters:**

1. `command instance` - sc-node that designate instance of a command. It should be included into
specified `command class` set.

Templates for a specified `command class` should be defined with relation `nrel_text_templates`.
This relation edge starts from templates set and end in `command class`. All of templates
included into set with an attribute relation. This attribute describes for which agent result it can be used. For example:

```js
command_add_into_set
    <- sc_node_not_relation;
    <- command;
    => nrel_common_template:
        [*
            command_add_into_set
                _-> _command;;

            _command
                _-> rrel_1:: _set;
                _-> rrel_2:: _element;;
        *];

    <= nrel_text_templates:
       ...
       (*
           <- sc_node_not_binary_tuple;;
           -> ...
           (*
               => nrel_suitable_for:
                   sc_result_ok;;
               -> [Tanks for an information] (* <- lang_en;; *);;
               -> [Спасибо за информацию] (* <- lang_ru;; *);;
           *);;
           -> ...
           (*
               => nrel_suitable_for:
                   sc_result_invalid_state;;
               -> [Yes, i know this] (* <- lang_en;; *);;
               -> [Да, я это знаю] (* <- lang_ru;; *);;
           *);;
       *);;
```

**Algorithm:**

When agent try to generate text, it find template with a specified command result and language.
If there are more then one suitable templates, then any of them will be used.
For example if command finished with `sc_result_ok` status and user use `lang_en` language,
then in our sample template `[Tanks for an information]` will be used.

<div class="note">If there are no specified template for a <code>sc_result_error_...</code>, then
<code>sc_result_error</code> will be used</div>

<div class="note"><b>TODO:</b> More complex description </div>

### Example

<div class="note"><b>TODO:</b> Add examples</div>

### Result

Result contains one sc-link with generated text.

Possible result codes:

* `sc_result_ok` - text generated;
* `sc_result_error_invalid_params` - there are no `command instance` in parameters;
* `sc_result_error_invalid_state` - invalid state of knowledge base (in most cases bad description of templates);
* `sc_result_error_not_found` - there are no any template found;
* `sc_result_error` - unknown error.

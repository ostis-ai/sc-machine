/*
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010-2013 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
*/

#include "scp_types.h"

#include "sc_memory_headers.h"

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

//sc_addr format_numeric;

#define NUMBER_PRECISE 8

scp_result check_type(sc_memory_context *context, sc_addr element, sc_type input_type)
{
    sc_type type;
    if (SC_RESULT_OK != sc_memory_get_element_type(context, element, &type))
    {
        return SCP_RESULT_ERROR;
    }
    if ((input_type & type) == input_type)
    {
        return SCP_RESULT_TRUE;
    }
    else
    {
        return SCP_RESULT_FALSE;
    }
    return SCP_RESULT_ERROR;
}

scp_result print_error(const char *operator_name, const char *text)
{
    printf("\nSCP LIBRARY ERROR: %s : %s\n", operator_name, text);
    return SCP_RESULT_ERROR;
}

scp_result print_parameter_error(const char *operator_name, const char *parameter_name, const char *text)
{
    printf("\nSCP LIBRARY ERROR: %s : %s %s\n", operator_name, parameter_name, text);
    return SCP_RESULT_ERROR;
}

#ifdef SCP_MATH
scp_result check_numeric_type(sc_memory_context *context, sc_addr param)
{
    //! TODO Add check for numeric type
    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_MATH
scp_result resolve_number(sc_memory_context *context, const sc_char *operator_name, const sc_char *parameter_name, scp_operand *param, double *num)
{
    sc_stream *stream;
    sc_uint32 length = 0, read_length = 0;
    sc_char *data1;

    if (SCP_ASSIGN == param->param_type)
    {
        return print_parameter_error(operator_name, parameter_name, "must have FIXED modifier");
    }

    if (SC_FALSE == sc_memory_is_element(context, param->addr))
    {
        return print_parameter_error(operator_name, parameter_name, "has not value");
    }

    if (SCP_RESULT_FALSE == check_type(context, param->addr, scp_type_link))
    {
        return print_parameter_error(operator_name, parameter_name, "must have link type");
    }

    if (SCP_RESULT_FALSE == check_numeric_type(context, param->addr))
    {
        return print_parameter_error(operator_name, parameter_name, "must have numeric format");
    }

    if (SC_RESULT_OK != sc_memory_get_link_content(context, param->addr, &stream))
    {
        return print_parameter_error(operator_name, parameter_name, "content reading error");
    }

    sc_stream_get_length(stream, &length);
    data1 = calloc(length, sizeof(sc_char));
    sc_stream_read_data(stream, data1, length, &read_length);
    sc_stream_free(stream);

    *num = atof(data1);
    free(data1);

    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_MATH
scp_result resolve_numbers_1_2(sc_memory_context *context, const sc_char *operator_name, scp_operand *param1, scp_operand *param2, double *num1, double *num2)
{
    if (SCP_RESULT_ERROR == resolve_number(context, operator_name, "Parameter 1", param1, num1))
    {
        return SCP_RESULT_ERROR;
    }

    if (SCP_RESULT_ERROR == resolve_number(context, operator_name, "Parameter 2", param2, num2))
    {
        return SCP_RESULT_ERROR;
    }

    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_MATH
scp_result resolve_numbers_2_3(sc_memory_context *context, const sc_char *operator_name, scp_operand *param2, scp_operand *param3, double *num2, double *num3)
{
    if (SCP_RESULT_ERROR == resolve_number(context, operator_name, "Parameter 2", param2, num2))
    {
        return SCP_RESULT_ERROR;
    }

    if (SCP_RESULT_ERROR == resolve_number(context, operator_name, "Parameter 3", param3, num3))
    {
        return SCP_RESULT_ERROR;
    }

    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_MATH
scp_result resolve_number_2(sc_memory_context *context, const sc_char *operator_name, scp_operand *param1, double *num1)
{
    return resolve_number(context, operator_name, "Parameter 2", param1, num1);
}
#endif

#ifdef SCP_MATH
scp_result write_link_content_number(sc_memory_context *context, double data, sc_addr link)
{
    sc_stream *stream;
    char *content = calloc(NUMBER_PRECISE, sizeof(sc_char));
    g_snprintf(content, NUMBER_PRECISE, "%lf", data);
    stream = sc_stream_memory_new(content, (sc_uint)strlen(content), SC_STREAM_FLAG_READ, SC_FALSE);
    if (SC_RESULT_OK != sc_memory_set_link_content(context, link, stream))
    {
        free(content);
        sc_stream_free(stream);
        return SCP_RESULT_ERROR;
    }
    free(content);
    sc_stream_free(stream);
    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_STRING
scp_result check_string_type(sc_memory_context *context, sc_addr param)
{
    //! TODO Add check for string type
    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_STRING
scp_result write_link_content_string(sc_memory_context *context, char* data, sc_addr link)
{
    sc_stream *stream;
    size_t data_len = strlen(data) + 1;
    char *content = calloc(data_len, sizeof(sc_char));
    g_snprintf(content, (gulong)data_len, "%s", data);
    stream = sc_stream_memory_new(content, (sc_uint)strlen(content), SC_STREAM_FLAG_READ, SC_FALSE);
    if (SC_RESULT_OK != sc_memory_set_link_content(context, link, stream))
    {
        free(content);
        sc_stream_free(stream);
        return SCP_RESULT_ERROR;
    }
    free(content);
    sc_stream_free(stream);
    return SCP_RESULT_TRUE;
}
#endif

scp_result check_link_parameter_1(sc_memory_context *context, const sc_char *operator_name, scp_operand *param1)
{
    if (SCP_ASSIGN == param1->param_type)
    {
        param1->addr = sc_memory_link_new(context);
        //! TODO Add numeric class for link
    }
    else
    {
        if (SC_FALSE == sc_memory_is_element(context, param1->addr))
        {
            return print_error(operator_name, "Parameter 1 has not value");
        }
        if (check_type(context, param1->addr, scp_type_link) == SCP_RESULT_FALSE)
        {
            return print_error(operator_name, "Parameter 1 must have link type");
        }
    }
    return SCP_RESULT_TRUE;
}

scp_result check_node_parameter_1(sc_memory_context *context, const sc_char *operator_name, scp_operand *param1)
{
    if (SCP_ASSIGN == param1->param_type)
    {
        param1->addr = sc_memory_node_new(context, scp_type_node);
    }
    else
    {
        if (SC_FALSE == sc_memory_is_element(context, param1->addr))
        {
            return print_error(operator_name, "Parameter 1 has not value");
        }
        if (check_type(context, param1->addr, scp_type_node) == SCP_RESULT_FALSE)
        {
            return print_error(operator_name, "Parameter 1 isn't node");
        }
    }
    return SCP_RESULT_TRUE;
}

#ifdef SCP_STRING
static scp_result resolve_string(sc_memory_context *context, const sc_char *operator_name, const sc_char *parameter_name, scp_operand *param, char **str)
{
    sc_stream *stream;
    sc_uint32 length = 0, read_length = 0;
    sc_char *data;

    if (SCP_ASSIGN == param->param_type)
    {
        return print_parameter_error(operator_name, parameter_name, "must have FIXED modifier");
    }

    if (SC_FALSE == sc_memory_is_element(context, param->addr))
    {
        return print_parameter_error(operator_name, parameter_name, "has not value");
    }

    if (SCP_RESULT_FALSE == check_type(context, param->addr, scp_type_link))
    {
        return print_parameter_error(operator_name, parameter_name, "must have link type");
    }

    if (SCP_RESULT_FALSE == check_string_type(context, param->addr))
    {
        return print_parameter_error(operator_name, parameter_name, "must have string format");
    }

    if (SC_RESULT_OK != sc_memory_get_link_content(context, param->addr, &stream))
    {
        return print_parameter_error(operator_name, parameter_name, "content reading error");
    }

    sc_stream_get_length(stream, &length);
    // extra byte needed for string terminator, which is neither counted nor read from an sc_stream
    data = calloc(length + 1, sizeof(sc_char));
    sc_stream_read_data(stream, data, length, &read_length);
    sc_stream_free(stream);

    size_t str_length = strlen(data) + 1;
    *str = calloc(str_length, sizeof(sc_char));
    memcpy(*str, data, str_length);
    free(data);

    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_STRING
scp_result resolve_string_2(sc_memory_context *context, const sc_char *operator_name, scp_operand *param2, char **str2) {
    return resolve_string(context, operator_name, "Parameter 2", param2, str2);
}
#endif

#ifdef SCP_STRING
scp_result resolve_strings_1_2(sc_memory_context *context, const sc_char *operator_name, scp_operand *param1,
                               scp_operand *param2, char **str1, char **str2)
{
    if (SCP_RESULT_ERROR == resolve_string(context, operator_name, "Parameter 1", param1, str1))
    {
        return SCP_RESULT_ERROR;
    }

    if (SCP_RESULT_ERROR == resolve_string(context, operator_name, "Parameter 2", param2, str2))
    {
        free(*str1);
        return SCP_RESULT_ERROR;
    }

    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_STRING
scp_result resolve_strings_2_3_4(sc_memory_context *context, const sc_char *operator_name, scp_operand *param2,
                               scp_operand *param3, scp_operand *param4, char **str1, char **str2, char **str3)
{
    if (SCP_RESULT_ERROR == resolve_string(context, operator_name, "Parameter 2", param2, str1))
    {
        return SCP_RESULT_ERROR;
    }

    if (SCP_RESULT_ERROR == resolve_string(context, operator_name, "Parameter 3", param3, str2))
    {
        free(*str1);
        return SCP_RESULT_ERROR;
    }

    if (SCP_RESULT_ERROR == resolve_string(context, operator_name, "Parameter 4", param4, str3))
    {
        free(*str1);
        free(*str2);
        return SCP_RESULT_ERROR;
    }

    return SCP_RESULT_TRUE;
}
#endif

#ifdef SCP_STRING
scp_result resolve_strings_2_3(sc_memory_context *context, const sc_char *operator_name, scp_operand *param2,
                               scp_operand *param3, char **str2, char **str3)
{
    if (SCP_RESULT_ERROR == resolve_string(context, operator_name, "Parameter 2", param2, str2))
    {
        return SCP_RESULT_ERROR;
    }

    if (SCP_RESULT_ERROR == resolve_string(context, operator_name, "Parameter 3", param3, str3))
    {
        free(*str2);
        return SCP_RESULT_ERROR;
    }

    return SCP_RESULT_TRUE;
}
#endif

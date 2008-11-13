/*
 *
 * Header file for Hugin API, version 4.1.3.
 *
 * Copyright (C) 1989-1999, Hugin Expert A/S. All Rights Reserved.
 *
 *
 * This file can be used with both ISO (ANSI) C and C++ compilers.
 * C++ compilers should define the __cplusplus preprocessor symbol.
 *
 */
# ifndef __hugin_h
# define __hugin_h
# ifdef __cplusplus
extern"C"{
# endif
# include <stdio.h>
# include <stddef.h>
typedef ptrdiff_t h_count_t,h_index_t;
typedef int h_boolean_t;
typedef int h_status_t;
typedef char*h_string_t;
typedef size_t h_identifier_t;
typedef struct _h_domain_structure*h_domain_t;
typedef struct _h_jt_structure*h_junction_tree_t;
typedef struct _h_node_structure*h_node_t;
typedef struct _h_clique_structure*h_clique_t;
typedef struct _h_table_structure*h_table_t;
typedef void(*h_parse_error_handler_t)(h_string_t);
typedef h_parse_error_handler_t h_progress_handler_t;
# include <float.h>
# ifdef H_DOUBLE
typedef double h_number_t,h_double_t;
# define h_infinity DBL_MAX
# else
#   ifdef H_QUADRUPLE
typedef long double h_number_t,h_double_t;
#   define h_infinity LDBL_MAX
#   else
typedef float h_number_t;
typedef double h_double_t;
#   define h_infinity DBL_MAX
#   endif
# endif
typedef enum
{
h_error_none= 0,
h_error_usage,
h_error_io,
h_error_no_memory,
h_error_twice,
h_error_invalid_name,
h_error_already_connected,
h_error_size_too_large,
h_error_fast_retraction,
h_error_zero_variance,
h_error_rounding,
h_error_bad_file,h_error_bad_version,h_error_format,
h_error_not_compiled,
h_error_cyclic_network,
h_error_decision_order,
h_error_enumeration,
h_error_zero_row_sum,
h_error_negative_probability,
h_error_chain_graph,
h_error_numbering_unknown_node,
h_error_numbering_twice,
h_error_numbering_bad_number,
h_error_no_equilibrium,
h_error_inconsistency_or_underflow,
h_error_overflow,
h_error_low_density,
h_error_cg_evidence_incorporated,
h_error_normalization,
h_error_compressed,
h_error_table_too_large,
h_error_parse,
h_error_locale,
h_error_invalid_expression,
h_error_syntax,
h_error_division_by_zero,h_error_inappropriate_arguments,
h_error_computation_failed,h_error_insufficient_state_range,
h_error_invalid_state_values,
h_error_serene,
h_error_no_current_domain,
h_error_unknown_state,
h_error_error= -1
}h_error_t;
typedef enum
{h_unused= 0,h_kind_discrete= 0,h_kind_continuous,h_kind_error
}h_node_kind_t;
typedef enum
{h_category_error= -1,
h_category_chance= 7,
h_category_decision,
h_category_utility
}h_node_category_t;
typedef long h_coordinate_t;
typedef struct _h_chain_block_structure*h_chain_block_t;
typedef enum{h_endian_host,h_endian_big,h_endian_little}h_endian_t;
typedef enum
{h_tm_clique_size= 15,h_tm_clique_weight,
h_tm_fill_in_size,h_tm_fill_in_weight
}h_triangulation_method_t;
typedef enum
{tm_max_card,tm_own,
tm_min_size= h_tm_clique_size,
tm_min_weight= h_tm_clique_weight,
tm_min_fill_in= h_tm_fill_in_size,
tm_min_fill_in_weight= h_tm_fill_in_weight,
tm_default= h_tm_fill_in_weight
}h_triang_method_t;
typedef enum
{h_equilibrium_none= 10,
h_equilibrium_sum,
h_equilibrium_max
}h_equilibrium_t;
typedef enum{h_mode_normal= 20,h_mode_fast_retraction}h_evidence_mode_t;
typedef struct _h_stream_structure*h_stream_t;
typedef enum
{
h_stream_is_end_of_file,
h_stream_is_error,
h_stream_is_valid
}h_stream_status_t;
typedef struct _h_new_table_structure*h_table_reference;
typedef struct _h_control_value*h_table_control;
typedef struct _h_random_structure*h_random_stream_t;
typedef struct _h_batch_structure*h_batch_t;
typedef void(*h_error_handler_t)(int,h_string_t);
typedef void(*h_error_handler_with_state_t)(int,h_string_t,void*);
typedef enum
{h_subtype_label= 30,h_subtype_boolean,h_subtype_number,
h_subtype_interval,h_subtype_error
}h_node_subtype_t;
typedef struct _h_expr_structure*h_expression_t;
typedef enum
{h_operator_Normal= 60,h_operator_Beta,h_operator_Gamma,
h_operator_Exponential,h_operator_Weibull,h_operator_Uniform,
h_operator_Binomial,h_operator_Poisson,h_operator_Geometric,
h_operator_NoisyOR,
h_operator_min,h_operator_max,h_operator_log,h_operator_exp,
h_operator_sqrt,
h_operator_if,h_operator_and,h_operator_or,h_operator_not,
h_operator_add,h_operator_subtract,h_operator_multiply,
h_operator_divide,h_operator_power,h_operator_negate,
h_operator_equals,h_operator_less_than,h_operator_greater_than,
h_operator_not_equals,h_operator_less_than_or_equals,
h_operator_greater_than_or_equals,
h_operator_number,h_operator_label,h_operator_node,h_operator_boolean,
h_operator_error
}h_operator_t;
typedef struct _h_model_structure*h_model_t;
typedef h_error_t hugin_error_type;
typedef h_node_t node_reference;
typedef h_node_t*node_set;
typedef h_number_t*h_belief_vector;
typedef h_clique_t*clique_set;
typedef h_domain_t domain_reference;
typedef h_clique_t clique_reference;
typedef h_triang_method_t h_triang_method;
typedef h_parse_error_handler_t h_parse_error_handler;
typedef h_stream_t h_stream_reference;
typedef h_stream_status_t h_stream_status;
typedef h_batch_t h_batch_reference;
typedef h_random_stream_t h_random_reference;
# define herr_none h_error_none
# define herr_usage h_error_usage
# define herr_notfound h_error_error
# define herr_twice h_error_twice
# define herr_nomem h_error_no_memory
# define herr_format h_error_format
# define herr_nodomain h_error_no_current_domain
# define herr_nonode h_error_error
# define herr_nostate h_error_unknown_state
# define herr_noclique h_error_error
# define herr_emptynet h_error_error
# define herr_badfile h_error_bad_file
# define herr_badversion h_error_bad_version
# define herr_ioerr h_error_io
# define herr_badname h_error_error
# define herr_parse h_error_parse
# define herr_numbering_unknown_node h_error_numbering_unknown_node
# define herr_numbering_twice h_error_numbering_twice
# define herr_numbering_bad_number h_error_numbering_bad_number
# define herr_compressed h_error_compressed
# define herr_end h_error_error
# define herr_error h_error_error
# undef _H_DEF
# define _H_DEF(func, args) extern func args
_H_DEF(h_error_t h_error_code,(void));
_H_DEF(h_string_t h_error_name,(h_error_t));
_H_DEF(h_string_t h_error_string,(void));
_H_DEF(h_string_t h_error_description,(h_error_t));
_H_DEF(h_node_t h_domain_new_node,
(h_domain_t,h_node_category_t,h_node_kind_t));
_H_DEF(h_node_category_t h_node_get_category,(h_node_t));
_H_DEF(h_node_kind_t h_node_get_kind,(h_node_t));
_H_DEF(h_domain_t h_node_get_domain,(h_node_t));
_H_DEF(h_node_t h_domain_get_node_by_name,(h_domain_t,h_string_t));
_H_DEF(h_status_t h_node_set_name,(h_node_t,h_string_t));
_H_DEF(h_string_t h_node_get_name,(h_node_t));
_H_DEF(h_status_t h_node_set_number_of_states,(h_node_t,size_t));
_H_DEF(h_count_t h_node_get_number_of_states,(h_node_t));
_H_DEF(h_status_t h_node_set_state_label,(h_node_t,size_t,h_string_t));
_H_DEF(h_status_t h_node_delete,(h_node_t));
_H_DEF(h_status_t h_domain_add_chain_link,(h_node_t,h_node_t));
_H_DEF(h_status_t h_node_add_parent,(h_node_t,h_node_t));
_H_DEF(h_status_t h_node_remove_parent,(h_node_t,h_node_t));
_H_DEF(h_status_t h_node_reverse_edge,(h_node_t,h_node_t));
_H_DEF(h_node_t h_domain_get_first_node,(h_domain_t));
_H_DEF(h_node_t h_node_get_next,(h_node_t));
_H_DEF(h_count_t h_domain_get_number_of_nodes,(h_domain_t));
_H_DEF(h_count_t h_domain_get_number_of_discrete_nodes,(h_domain_t));
_H_DEF(h_count_t h_domain_get_number_of_continuous_nodes,(h_domain_t));
_H_DEF(h_count_t h_domain_get_number_of_chance_nodes,(h_domain_t));
_H_DEF(h_count_t h_domain_get_number_of_decision_nodes,(h_domain_t));
_H_DEF(h_count_t h_domain_get_number_of_utility_nodes,(h_domain_t));
_H_DEF(h_string_t h_node_get_label,(h_node_t));
_H_DEF(h_status_t h_node_set_label,(h_node_t,h_string_t));
_H_DEF(h_status_t h_node_get_position,
(h_node_t,h_coordinate_t*,h_coordinate_t*));
_H_DEF(h_status_t h_node_set_position,
(h_node_t,h_coordinate_t,h_coordinate_t));
_H_DEF(h_string_t h_node_get_state_label,(h_node_t,size_t));
_H_DEF(h_count_t h_node_get_number_of_parents,(h_node_t));
_H_DEF(h_count_t h_node_get_number_of_children,(h_node_t));
_H_DEF(h_node_t*h_node_get_parents,(h_node_t));
_H_DEF(h_node_t*h_node_get_children,(h_node_t));
_H_DEF(h_table_t h_node_get_table,(h_node_t));
_H_DEF(h_status_t h_table_reorder_nodes,(h_table_t,h_node_t*));
_H_DEF(h_status_t h_node_set_alpha,(h_node_t,h_double_t,size_t));
_H_DEF(h_status_t h_node_set_beta,(h_node_t,h_double_t,h_node_t,size_t));
_H_DEF(h_status_t h_node_set_gamma,(h_node_t,h_double_t,size_t));
_H_DEF(h_double_t h_node_get_alpha,(h_node_t,size_t));
_H_DEF(h_double_t h_node_get_beta,(h_node_t,h_node_t,size_t));
_H_DEF(h_double_t h_node_get_gamma,(h_node_t,size_t));
_H_DEF(h_clique_t h_domain_get_first_clique,(h_domain_t));
_H_DEF(h_clique_t h_clique_get_next,(h_clique_t));
_H_DEF(h_count_t h_domain_get_number_of_cliques,(h_domain_t));
_H_DEF(h_identifier_t h_clique_id,(h_clique_t));
_H_DEF(h_junction_tree_t h_clique_get_junction_tree,(h_clique_t));
_H_DEF(h_node_t*h_clique_get_members,(h_clique_t));
_H_DEF(h_count_t h_clique_get_number_of_neighbors,(h_clique_t));
_H_DEF(h_count_t h_clique_get_number_of_members,(h_clique_t));
_H_DEF(h_clique_t*h_clique_get_neighbors,(h_clique_t));
_H_DEF(h_table_t h_clique_get_probability_table,(h_clique_t));
_H_DEF(h_table_t h_clique_get_utility_table,(h_clique_t));
_H_DEF(h_domain_t h_new_domain,(void));
_H_DEF(h_junction_tree_t h_domain_get_first_junction_tree,(h_domain_t));
_H_DEF(h_junction_tree_t h_jt_get_next,(h_junction_tree_t));
_H_DEF(h_clique_t h_jt_get_root,(h_junction_tree_t));
_H_DEF(h_clique_t*h_jt_get_cliques,(h_junction_tree_t));
_H_DEF(h_status_t h_domain_set_node_size,(h_domain_t,size_t,size_t));
_H_DEF(h_status_t h_domain_get_node_size,(h_domain_t,size_t*,size_t*));
_H_DEF(h_string_t h_domain_get_filename,(h_domain_t));
_H_DEF(h_string_t h_domain_get_file_name,(h_domain_t));
_H_DEF(h_node_t*h_table_get_nodes,(h_table_t));
_H_DEF(h_number_t*h_table_get_data,(h_table_t));
_H_DEF(size_t h_table_get_size,(h_table_t));
_H_DEF(h_double_t h_table_get_mean,(h_table_t,size_t,h_node_t));
_H_DEF(h_double_t h_table_get_covariance,
(h_table_t,size_t,h_node_t,h_node_t));
_H_DEF(h_double_t h_table_get_variance,(h_table_t,size_t,h_node_t));
_H_DEF(h_status_t h_table_delete,(h_table_t));
_H_DEF(h_status_t h_domain_delete,(h_domain_t));
_H_DEF(h_status_t h_domain_uncompile,(h_domain_t));
_H_DEF(h_status_t h_domain_save,(h_domain_t,h_string_t,h_endian_t));
_H_DEF(h_domain_t h_load_domain,(h_string_t));
_H_DEF(h_status_t h_domain_set_log_file,(h_domain_t,FILE*));
_H_DEF(h_boolean_t h_domain_is_compiled,(h_domain_t));
_H_DEF(h_status_t h_domain_compile,(h_domain_t));
_H_DEF(h_status_t h_domain_compile_qualitatively,
(h_domain_t,h_progress_handler_t));
_H_DEF(h_status_t h_domain_triangulate,
(h_domain_t,h_triangulation_method_t));
_H_DEF(h_boolean_t h_domain_is_triangulated,(h_domain_t));
_H_DEF(h_status_t h_domain_triangulate_with_order,
(h_domain_t,h_node_t*));
_H_DEF(h_node_t*h_domain_get_elimination_order,(h_domain_t));
_H_DEF(h_junction_tree_t h_node_get_junction_tree,(h_node_t));
_H_DEF(h_status_t h_domain_compile_quantitatively,
(h_domain_t,h_progress_handler_t));
_H_DEF(h_domain_t h_compile,
(FILE*,FILE*,h_triang_method_t,h_parse_error_handler_t,FILE*,
h_progress_handler_t));
_H_DEF(h_domain_t h_compile_domain,
(FILE*,FILE*,h_triang_method_t,h_parse_error_handler_t));
_H_DEF(h_status_t h_domain_write_node_numbering,(h_domain_t,FILE*));
_H_DEF(h_boolean_t h_node_evidence_to_retract,(h_node_t));
_H_DEF(h_boolean_t h_node_evidence_to_propagate,(h_node_t));
_H_DEF(h_boolean_t h_node_evidence_is_entered,(h_node_t));
_H_DEF(h_boolean_t h_node_likelihood_is_entered,(h_node_t));
_H_DEF(h_boolean_t h_node_evidence_is_propagated,(h_node_t));
_H_DEF(h_boolean_t h_node_likelihood_is_propagated,(h_node_t));
_H_DEF(h_boolean_t h_node_is_instantiated,(h_node_t));
_H_DEF(h_index_t h_node_which_state,(h_node_t));
_H_DEF(h_boolean_t h_domain_evidence_to_propagate,(h_domain_t));
_H_DEF(h_boolean_t h_jt_evidence_to_propagate,(h_junction_tree_t));
_H_DEF(h_boolean_t h_domain_evidence_is_propagated,(h_domain_t));
_H_DEF(h_boolean_t h_jt_evidence_is_propagated,(h_junction_tree_t));
_H_DEF(h_boolean_t h_domain_likelihood_is_propagated,(h_domain_t));
_H_DEF(h_boolean_t h_domain_cg_evidence_is_propagated,(h_domain_t));
_H_DEF(h_boolean_t h_jt_likelihood_is_propagated,(h_junction_tree_t));
_H_DEF(h_boolean_t h_jt_cg_evidence_is_propagated,(h_junction_tree_t));
_H_DEF(h_boolean_t h_node_is_consistent,(h_node_t));
_H_DEF(h_status_t h_node_enter_finding,(h_node_t,size_t,h_number_t));
_H_DEF(h_status_t h_node_enter_value,(h_node_t,h_double_t));
_H_DEF(h_status_t h_node_select_state,(h_node_t,size_t));
_H_DEF(h_status_t h_node_retract_findings,(h_node_t));
_H_DEF(h_status_t h_domain_retract_findings,(h_domain_t));
_H_DEF(h_number_t h_node_get_entered_finding,(h_node_t,size_t));
_H_DEF(h_number_t h_node_get_propagated_finding,(h_node_t,size_t));
_H_DEF(h_double_t h_node_get_entered_value,(h_node_t));
_H_DEF(h_double_t h_node_get_propagated_value,(h_node_t));
_H_DEF(h_status_t h_node_touch_table,(h_node_t));
_H_DEF(h_boolean_t h_domain_tables_to_propagate,(h_domain_t));
_H_DEF(h_boolean_t h_jt_tables_to_propagate,(h_junction_tree_t));
_H_DEF(h_status_t h_domain_reset_inference_engine,(h_domain_t));
_H_DEF(h_number_t h_node_get_belief,(h_node_t,size_t));
_H_DEF(h_double_t h_node_get_mean,(h_node_t));
_H_DEF(h_double_t h_node_get_variance,(h_node_t));
_H_DEF(h_number_t h_node_get_expected_utility,(h_node_t,size_t));
_H_DEF(h_table_t h_node_get_distribution,(h_node_t));
_H_DEF(h_double_t h_table_compute_density,(h_table_t,h_double_t));
_H_DEF(h_status_t h_domain_propagate,
(h_domain_t,h_equilibrium_t,h_evidence_mode_t));
_H_DEF(h_status_t h_jt_propagate,
(h_junction_tree_t,h_equilibrium_t,h_evidence_mode_t));
_H_DEF(h_status_t h_clique_propagate,
(h_clique_t,h_equilibrium_t,h_evidence_mode_t));
_H_DEF(h_boolean_t h_domain_equilibrium_is,(h_domain_t,h_equilibrium_t));
_H_DEF(h_boolean_t h_jt_equilibrium_is,(h_junction_tree_t,h_equilibrium_t));
_H_DEF(h_boolean_t h_domain_evidence_mode_is,
(h_domain_t,h_evidence_mode_t));
_H_DEF(h_boolean_t h_jt_evidence_mode_is,
(h_junction_tree_t,h_evidence_mode_t));
_H_DEF(h_double_t h_domain_get_normalization_constant,(h_domain_t));
_H_DEF(h_status_t h_domain_initialize,(h_domain_t));
_H_DEF(h_double_t h_clique_get_conflict,(h_clique_t));
_H_DEF(h_double_t h_domain_get_conflict,(h_domain_t));
_H_DEF(h_double_t h_jt_get_conflict,(h_junction_tree_t));
_H_DEF(h_table_t h_domain_get_marginal,(h_domain_t,h_node_t*));
_H_DEF(h_status_t h_domain_save_to_memory,(h_domain_t));
_H_DEF(h_status_t h_domain_reclaim_memory_tables,(h_domain_t));
_H_DEF(h_boolean_t h_domain_is_compressed,(h_domain_t));
_H_DEF(h_double_t h_domain_compress,(h_domain_t));
_H_DEF(h_double_t h_domain_get_approximation_constant,(h_domain_t));
_H_DEF(h_double_t h_domain_approximate,(h_domain_t,h_double_t));
_H_DEF(h_stream_t h_stream_open,(void*,h_string_t));
_H_DEF(h_stream_status_t h_stream_check,(h_stream_t));
_H_DEF(void h_stream_set_check,(h_stream_t,h_stream_status_t(*)(void*)));
_H_DEF(int h_stream_get_character,(h_stream_t));
_H_DEF(void h_stream_set_get_character,(h_stream_t,int(*)(void*)));
_H_DEF(void h_stream_put_character,(h_stream_t,int));
_H_DEF(void h_stream_set_put_character,(h_stream_t,void(*)(void*,int)));
_H_DEF(void h_stream_flush,(h_stream_t));
_H_DEF(void h_stream_set_flush,(h_stream_t,void(*)(void*)));
_H_DEF(void h_stream_rewind,(h_stream_t));
_H_DEF(void h_stream_set_rewind,(h_stream_t,void(*)(void*)));
_H_DEF(void h_stream_truncate,(h_stream_t));
_H_DEF(void h_stream_set_truncate,(h_stream_t,void(*)(void*)));
_H_DEF(void h_stream_close,(h_stream_t));
_H_DEF(void h_stream_set_close,(h_stream_t,void(*)(void*)));
_H_DEF(void h_stream_unget_character,(h_stream_t));
_H_DEF(int h_stream_get_line,(h_stream_t));
_H_DEF(int h_stream_get_column,(h_stream_t));
_H_DEF(h_string_t h_stream_get_name,(h_stream_t));
_H_DEF(h_stream_t h_stream_open_stdio,(FILE*,h_string_t));
_H_DEF(h_stream_t h_stream_open_file,(h_string_t,h_string_t));
_H_DEF(h_stream_t h_stream_open_memory,(void));
_H_DEF(h_string_t h_stream_memory_string,(h_stream_t));
_H_DEF(h_stream_t h_stream_open_function,(void(*)(h_string_t)));
_H_DEF(h_boolean_t h_stream_function_is_touched,(h_stream_t));
_H_DEF(void h_stream_print_number,(h_stream_t,double));
_H_DEF(void h_stream_print_integer,(h_stream_t,int));
_H_DEF(void h_stream_print_string,(h_stream_t,h_string_t));
_H_DEF(void h_stream_report,(h_stream_t,h_stream_t,h_string_t));
_H_DEF(h_double_t h_stream_read_number,(h_stream_t,h_stream_t));
_H_DEF(void h_stream_skip_whitespace,(h_stream_t));
_H_DEF(void h_stream_read_string,(h_stream_t,h_stream_t,h_stream_t));
_H_DEF(h_table_reference h_table_create,(void*));
_H_DEF(h_table_reference h_table_create_structure,(void*));
_H_DEF(h_table_reference h_table_partial,(h_table_reference));
_H_DEF(h_status_t h_table_allocate,(h_table_reference));
_H_DEF(h_status_t h_table_set_all,(h_table_reference,h_number_t));
_H_DEF(h_status_t h_table_add_dimension,(h_table_reference,size_t,void*));
_H_DEF(void h_table_free,(h_table_reference));
_H_DEF(h_table_reference h_table_clone,(h_table_reference));
_H_DEF(h_boolean_t h_table_same_structure,
(h_table_reference,h_table_reference));
_H_DEF(h_status_t h_table_copy,(h_table_reference,h_table_reference));
_H_DEF(int h_table_compare,(h_table_reference,h_table_reference));
_H_DEF(h_status_t h_table_set_number_of_states,
(h_table_reference,size_t,size_t));
_H_DEF(h_status_t h_table_set_global_user_data,(h_table_reference,void*));
_H_DEF(void*h_table_get_global_user_data,(h_table_reference));
_H_DEF(h_status_t h_table_set_value,(h_table_reference,h_number_t));
_H_DEF(h_number_t h_table_get_value,(h_table_reference));
_H_DEF(h_count_t h_table_number_of_dimensions,(h_table_reference));
_H_DEF(h_status_t h_table_set_user_data,(h_table_reference,size_t,void*));
_H_DEF(void*h_table_get_user_data,(h_table_reference,size_t));
_H_DEF(h_status_t h_table_next_global_state,(h_table_reference));
_H_DEF(h_status_t h_table_previous_global_state,(h_table_reference));
_H_DEF(h_status_t h_table_select_global_state,
(h_table_reference,h_index_t));
_H_DEF(h_index_t h_table_chosen_global_state,(h_table_reference));
_H_DEF(size_t h_table_global_number_of_states,(h_table_reference));
_H_DEF(int h_table_next_state,(h_table_reference,size_t));
_H_DEF(int h_table_previous_state,(h_table_reference,size_t));
_H_DEF(h_status_t h_table_select_state,
(h_table_reference,size_t,h_index_t));
_H_DEF(h_index_t h_table_chosen_state,(h_table_reference,size_t));
_H_DEF(h_count_t h_table_number_of_states,(h_table_reference,size_t));
_H_DEF(h_table_control h_table_create_control,
(h_table_reference,h_table_reference,int(*)(void*,void*)));
_H_DEF(void h_table_free_control,(h_table_control));
_H_DEF(int h_table_marginalize_with_control,
(h_table_reference,h_table_reference,h_table_control));
_H_DEF(h_table_reference h_table_marginalize,(h_table_reference));
_H_DEF(h_status_t h_table_print_stream,
(h_table_reference,h_stream_t,h_string_t,h_string_t,
h_string_t(*)(void*,int)));
_H_DEF(h_table_reference h_table_read_stream,(h_stream_t,h_stream_t));
_H_DEF(h_status_t h_table_print,
(h_table_reference,FILE*,h_string_t,h_string_t,h_string_t,
h_string_t(*)(void*,int)));
_H_DEF(h_table_reference h_table_read,(FILE*,void(*)(h_string_t)));
_H_DEF(h_status_t h_adapt_node_retrieve,(h_node_t));
_H_DEF(h_status_t h_domain_adapt_retrieve,(h_domain_t));
_H_DEF(h_status_t h_domain_adapt_disseminate,(h_domain_t));
_H_DEF(h_table_reference h_adapt_examine_experience,(h_node_t));
_H_DEF(h_table_reference h_adapt_examine_fading,(h_node_t));
_H_DEF(h_status_t h_domain_adapt_print_stream,(h_domain_t,h_stream_t));
_H_DEF(h_status_t h_domain_adapt_read_stream,
(h_domain_t,h_stream_t,h_stream_t));
_H_DEF(h_status_t h_domain_adapt_save,(h_domain_t,FILE*));
_H_DEF(h_status_t h_domain_adapt_load,
(h_domain_t,FILE*,void(*)(h_string_t)));
_H_DEF(h_random_stream_t h_random_create,(void));
_H_DEF(void h_random_free,(h_random_stream_t));
_H_DEF(void h_random_set_function,
(h_random_stream_t,double(*)(void*)));
_H_DEF(void h_random_set_data,(h_random_stream_t,void*));
_H_DEF(double h_random_number,(h_random_stream_t));
_H_DEF(h_random_stream_t h_random_default,(void));
_H_DEF(h_status_t h_domain_batch_generate_stream,
(h_domain_t,h_stream_t,h_string_t,h_node_t*,h_double_t*,size_t,
h_random_stream_t));
_H_DEF(h_status_t h_domain_batch_generate,
(h_domain_t,FILE*,h_string_t,h_node_t*,h_double_t*,size_t));
_H_DEF(h_batch_t h_domain_batch_open_stream,
(h_domain_t,h_stream_t,h_stream_t));
_H_DEF(void h_batch_close_stream,(h_batch_t));
_H_DEF(h_stream_t h_batch_get_input_stream,(h_batch_t));
_H_DEF(h_stream_t h_batch_get_error_stream,(h_batch_t));
_H_DEF(h_count_t h_batch_number_of_nodes,(h_batch_t));
_H_DEF(h_count_t h_batch_length,(h_batch_t));
_H_DEF(h_string_t h_batch_node_name,(h_batch_t,size_t));
_H_DEF(h_count_t h_batch_count,(h_batch_t,size_t));
_H_DEF(h_boolean_t h_batch_search,(h_batch_t));
_H_DEF(h_string_t h_batch_finding_name,(h_batch_t,size_t));
_H_DEF(h_batch_t h_domain_batch_open,
(h_domain_t,FILE*,h_string_t,void(*)(h_string_t)));
_H_DEF(int h_batch_close,(h_batch_t));
_H_DEF(h_status_t h_Domain_print_stream,
(h_domain_t,h_stream_t,h_boolean_t));
_H_DEF(h_status_t h_Domain_print,(h_domain_t,FILE*,h_boolean_t));
_H_DEF(h_double_t h_random,(void));
_H_DEF(void h_seed_random,(size_t));
_H_DEF(h_double_t h_domain_probability_of_random_selection,(h_domain_t));
_H_DEF(h_status_t h_domain_simulate,(h_domain_t));
_H_DEF(h_index_t h_node_get_selection,(h_node_t));
_H_DEF(h_domain_t h_net_parse,(FILE*,h_error_handler_t));
_H_DEF(h_domain_t h_net_parse_from_string,(h_string_t,h_error_handler_t));
_H_DEF(h_status_t h_domain_write_net,(h_domain_t,FILE*));
_H_DEF(h_count_t h_domain_write_case_file,(h_domain_t,FILE*));
_H_DEF(h_count_t h_domain_read_case_file,(h_domain_t,FILE*));
_H_DEF(h_status_t h_domain_set_user_data,(h_domain_t,void*));
_H_DEF(void*h_domain_get_user_data,(h_domain_t));
_H_DEF(h_status_t h_node_set_user_data,(h_node_t,void*));
_H_DEF(void*h_node_get_user_data,(h_node_t));
_H_DEF(h_status_t h_clique_set_user_data,(h_clique_t,void*));
_H_DEF(void*h_clique_get_user_data,(h_clique_t));
_H_DEF(h_status_t h_jt_set_user_data,(h_junction_tree_t,void*));
_H_DEF(void*h_jt_get_user_data,(h_junction_tree_t));
_H_DEF(h_status_t h_domain_set_attribute,
(h_domain_t,h_string_t,h_string_t));
_H_DEF(h_string_t h_domain_get_attribute,(h_domain_t,h_string_t));
_H_DEF(h_status_t h_node_set_attribute,(h_node_t,h_string_t,h_string_t));
_H_DEF(h_string_t h_node_get_attribute,(h_node_t,h_string_t));
_H_DEF(h_node_subtype_t h_node_get_subtype,(h_node_t));
_H_DEF(h_status_t h_node_set_subtype,(h_node_t,h_node_subtype_t));
_H_DEF(h_status_t h_node_set_state_value,(h_node_t,size_t,h_double_t));
_H_DEF(h_double_t h_node_get_state_value,(h_node_t,size_t));
_H_DEF(h_expression_t h_number_make_expression,(h_double_t));
_H_DEF(h_expression_t h_boolean_make_expression,(h_boolean_t));
_H_DEF(h_expression_t h_label_make_expression,(h_string_t));
_H_DEF(h_expression_t h_node_make_expression,(h_node_t));
_H_DEF(h_expression_t h_make_composite_expression,
(h_operator_t,h_expression_t*));
_H_DEF(h_boolean_t h_expression_is_composite,(h_expression_t));
_H_DEF(h_operator_t h_expression_get_operator,(h_expression_t));
_H_DEF(h_expression_t*h_expression_get_operands,(h_expression_t));
_H_DEF(h_node_t h_expression_get_node,(h_expression_t));
_H_DEF(h_double_t h_expression_get_number,(h_expression_t));
_H_DEF(h_string_t h_expression_get_label,(h_expression_t));
_H_DEF(h_boolean_t h_expression_get_boolean,(h_expression_t));
_H_DEF(h_expression_t h_expression_clone,(h_expression_t));
_H_DEF(h_status_t h_expression_delete,(h_expression_t));
_H_DEF(h_model_t h_node_new_model,(h_node_t,h_node_t*));
_H_DEF(h_model_t h_node_get_model,(h_node_t));
_H_DEF(h_node_t*h_model_get_nodes,(h_model_t));
_H_DEF(size_t h_model_get_size,(h_model_t));
_H_DEF(h_status_t h_model_delete,(h_model_t));
_H_DEF(h_string_t h_expression_to_string,(h_expression_t));
_H_DEF(h_status_t h_model_set_expression,(h_model_t,size_t,h_expression_t));
_H_DEF(h_expression_t h_model_get_expression,(h_model_t,size_t));
_H_DEF(h_expression_t h_string_to_expression,
(h_string_t,h_domain_t,h_error_handler_t));
_H_DEF(int h_check_license,(h_string_t*,int*,h_string_t*));
_H_DEF(h_status_t h_select_domain,(h_domain_t));
_H_DEF(h_domain_t h_current_domain,(void));
_H_DEF(h_status_t h_free_domain,(void));
_H_DEF(float h_node_width,(void));
_H_DEF(float h_node_height,(void));
_H_DEF(float h_scale_factor,(void));
_H_DEF(h_status_t h_save_domain,(h_string_t));
_H_DEF(h_string_t h_domain_file,(void));
_H_DEF(h_string_t h_domain_name,(void));
_H_DEF(h_string_t h_domain_label,(void));
_H_DEF(h_node_t h_get_node_by_name,(h_string_t));
_H_DEF(h_node_t h_first_node,(void));
_H_DEF(h_node_t h_next_node,(h_node_t));
_H_DEF(h_clique_t h_first_clique,(void));
_H_DEF(h_clique_t h_next_clique,(h_clique_t));
_H_DEF(h_clique_t*h_clique_neighbours,(h_clique_t));
_H_DEF(h_count_t h_clique_number_of_neighbours,(h_clique_t));
_H_DEF(h_node_t*h_clique_members,(h_clique_t));
_H_DEF(h_count_t h_clique_number_of_nodes,(h_clique_t));
_H_DEF(h_count_t h_number_of_nodes,(void));
_H_DEF(h_index_t h_node_id,(h_node_t));
_H_DEF(h_count_t h_node_number_of_states,(h_node_t));
_H_DEF(h_string_t h_node_name,(h_node_t));
_H_DEF(h_string_t h_node_label,(h_node_t));
_H_DEF(float h_node_x_coor,(h_node_t));
_H_DEF(float h_node_y_coor,(h_node_t));
_H_DEF(h_node_t*h_node_parents,(h_node_t));
_H_DEF(h_node_t*h_node_children,(h_node_t));
_H_DEF(h_count_t h_node_number_of_findings,(h_node_t));
_H_DEF(h_string_t*h_node_finding_states,(h_node_t));
_H_DEF(h_status_t h_retract_named_finding,(h_node_t,h_string_t));
_H_DEF(h_number_t*h_node_finding_vector,(h_node_t));
_H_DEF(h_status_t h_enter_finding,(h_node_t,h_string_t));
_H_DEF(h_status_t h_enter_additional_finding,(h_node_t,h_string_t));
_H_DEF(h_count_t h_retract_count,(void));
_H_DEF(h_status_t h_retract_finding,(h_node_t));
_H_DEF(h_number_t*h_get_belief,(h_node_t));
_H_DEF(h_status_t h_enter_likelihood,(h_node_t,h_number_t*));
_H_DEF(h_string_t*h_node_state_names,(h_node_t));
_H_DEF(h_string_t h_node_state_name,(h_node_t,size_t));
_H_DEF(h_error_t hugin_error,(void));
_H_DEF(h_string_t hugin_error_name,(h_error_t));
_H_DEF(h_string_t hugin_error_string,(void));
_H_DEF(h_string_t hugin_error_description,(h_error_t));
_H_DEF(h_double_t h_approximate_domain,(h_double_t));
_H_DEF(h_double_t h_compress_domain,(void));
_H_DEF(h_double_t h_normalization_constant,(void));
_H_DEF(h_double_t h_accumulated_constant,(void));
_H_DEF(h_boolean_t h_needs_propagation,(h_domain_t));
_H_DEF(h_status_t h_propagate,(void));
_H_DEF(h_status_t h_initialize_domain,(void));
_H_DEF(h_boolean_t h_is_compressed,(h_domain_t));
_H_DEF(h_status_t h_write_node_numbering,(FILE*));
_H_DEF(h_count_t h_number_of_cliques,(void));
_H_DEF(int h_write_case_file,(FILE*));
_H_DEF(int h_read_case_file,(FILE*));
_H_DEF(h_status_t h_adapt_retrieve,(void));
_H_DEF(h_status_t h_adapt_disseminate,(void));
_H_DEF(h_status_t h_adapt_print_stream,(h_stream_t));
_H_DEF(h_status_t h_batch_generate_stream,
(h_stream_t,h_string_t,h_node_t*,h_double_t*,size_t,
h_random_stream_t));
_H_DEF(h_status_t h_adapt_read_stream,(h_stream_t,h_stream_t));
_H_DEF(h_status_t h_adapt_save,(FILE*));
_H_DEF(h_status_t h_adapt_load,(FILE*,void(*)(h_string_t)));
_H_DEF(h_status_t h_batch_generate,
(FILE*,h_string_t,h_node_t*,h_double_t*,size_t));
_H_DEF(h_batch_t h_batch_open_stream,(h_stream_t,h_stream_t));
_H_DEF(h_batch_t h_batch_open,
(FILE*,h_string_t,void(*)(h_string_t)));
_H_DEF(h_status_t h_domain_print_stream,(h_stream_t,h_boolean_t));
_H_DEF(h_status_t h_domain_print,(FILE*,h_boolean_t));
_H_DEF(h_count_t h_node_number_of_parent_configurations,(h_node_t));
_H_DEF(h_count_t h_node_number_of_parents,(h_node_t));
_H_DEF(h_count_t h_node_number_of_children,(h_node_t));
_H_DEF(h_table_reference h_node_examine_potential,(h_node_t));
_H_DEF(h_table_reference h_clique_examine_table,(h_clique_t));
# undef _H_DEF
# ifdef __cplusplus
}
# endif
# endif 

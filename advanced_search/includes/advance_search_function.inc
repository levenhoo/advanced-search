<?php


/* 函数命名都应该带前缀 */

function _advanced_search_is_allow_type($type){
	$pass_type = array( "submit", "hidden" );
	return in_array( $type , $pass_type );
}
function _advanced_search_is_allow_key($key){
	$allow_key = array( "title", "submit", "form_build_id", "form_token","form_id" , "op");
	return in_array( $key , $allow_key );
}
function _advanced_search_is_allow_search($type){
	$allow_type = array( "text_textfield", "options_select", "options_buttons" );
	return in_array( $type , $allow_type );
}


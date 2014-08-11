<?php

    $form = $variables['form'];
    $content_type = isset( $form['content_type']['#value'] ) ? $form['content_type']['#value'] : "" ;

    $form_format = trim(variable_get("advanced_search_".$content_type));
    if(empty($form_format)){
        echo drupal_render_children($form);
    }
    preg_match_all (" /\[advance_search:\w*\]/ ",$form_format,$matches);
    if(count($matches)==1)
    foreach($matches[0] as $key => $element){
        $rest = substr($element,16, -1);
        $f = drupal_render($form[$rest]);
        $form_format = str_ireplace($element,$f, $form_format);
    }
    if(count($matches)==1)
    echo $form_format;
//echo drupal_render($form['submit']);
echo drupal_render($form['list']);
echo drupal_render_children($form);





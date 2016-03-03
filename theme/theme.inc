<?php

/**
 * @file
 * The theme system, which controls the output of Drupal.
 *
 * The theme system allows for nearly all output of the Drupal system to be
 * customized by user themes.
 */

/**
 * Implements hook_preprocess().
 */
function agile_harris_matrix_preprocess_agile_harris_matrix(&$variables) {
  module_load_include('inc', 'agile_harris_matrix', 'includes/utilities');
  $path = drupal_get_path('module', 'agile_harris_matrix');
  drupal_add_css("$path/css/harris_matrix.css");
  drupal_add_css("doc/css/reset.css");
  drupal_add_css("$path/js/tooltipster-master/doc/css/prettify.css");
  drupal_add_css("$path/js/tooltipster-master/doc/css/style.css");
  drupal_add_css("$path/js/tooltipster-master/css/tooltipster.css");
  drupal_add_css("$path/js/tooltipster-master/css/themes/tooltipster-light.css");
  drupal_add_css("$path/js/tooltipster-master/css/themes/tooltipster-noir.css");
  drupal_add_css("$path/js/tooltipster-master/css/themes/tooltipster-punk.css");
  drupal_add_css("$path/js/tooltipster-master/css/themes/tooltipster-shadow.css");
  $vocabs = taxonomy_get_vocabularies();
  $node = node_load($variables['nid']);
  $wrapper = entity_metadata_wrapper('node', $node);
  $term = $wrapper->field_taxonomy->value();
  foreach ($vocabs as $vocab) {
    if ($vocab->name == $term->name) {
      $vocabulary = taxonomy_vocabulary_machine_name_load($vocab->machine_name);
    }
  }
  if (!$vocabulary) {
    $vocabulary = taxonomy_vocabulary_machine_name_load('default_matrix_vocabulary');
  }

  drupal_add_js("$path/js/agile_harris_matrix.js");
  drupal_add_js("$path/js/klayjs-d3/dist/klayjs-d3-ww.js");
  drupal_add_js("$path/js/klayjs-d3/dist/klayjs-d3.js");
  drupal_add_js("$path/library/klayjs-d3/node_modules/klayjs/klay.js");
  drupal_add_js("$path/library/d3.js");

  drupal_add_js("$path/js/tooltipster-master/doc/js/jquery.jgfeed.js");
  drupal_add_js("$path/js/tooltipster-master/doc/js/prettify.js");
  drupal_add_js("$path/js/tooltipster-master/doc/js/lang-css.js");
  drupal_add_js("$path/js/tooltipster-master/js/jquery.tooltipster.js");
  $json = $wrapper->field_json->value();
  $tree = taxonomy_get_tree($vocabulary->vid);
  $current = array();
  foreach ($tree as $branch) {
    $current[] = $branch->name;
  }
  $candidates = agile_harris_get_terms($wrapper->getIdentifier());
  $new_terms = array_diff($candidates, $current);

  if (!$json) {
    $values = $wrapper->field_hmc->value();
    $xml_uri = $values['uri'];
    $realpath = drupal_realpath($xml_uri);
    $data = hmc2json($realpath);
    $unique_id = uniqid();
    $json_file = file_save_data($data, "public://{$unique_id}hm.json");
    $wrapper->field_json->file->set($json_file);
    $wrapper->save();
    $json = $wrapper->field_json->value();
    foreach ($new_terms as $new_term) {
      taxonomy_term_save((object) array(
            'name' => $new_term,
            'vid' => $vocabulary->vid,
      ));
    }
  }
  if (!isset($json['uri'])) {
    $file_object = file_load($json['fid']);
    $json['uri'] = $file_object->uri;
  }
  $url = file_create_url($json['uri']);
  drupal_add_js(array('agile_harris_matrix' => array('json_address' => $url, 'vocabulary' => $vocabulary->machine_name)), array('type' => 'setting'));
  $variables['title'] = $node->title;
}

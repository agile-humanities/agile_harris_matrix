<?php
/**
 * @file
 * Utility functions for this module.
 */

/**
 * Returns themed paged list of all Harris Matrix nodes.
 *
 * @return HTML
 *  Themed list
 */
function get_all_matrix_nodes() {
  $links = array();
  $query = new EntityFieldQuery();
  $result = $query->entityCondition('entity_type', 'node')
      ->entityCondition('bundle', 'harris_matrix')
      ->pager(10)
      ->execute();
  $nodes = node_load_multiple(array_keys($result['node']));
  foreach ($nodes as $node) {
    $links[] = l($node->title, "agile/matrix/{$node->nid}");
  }
  $variables = array(
    'items' => $links,
    'title' => t('Harris Matrix Visualizations'),
    'type' => 'ul',
    'attributes' => array('class' => 'harris_matrix_nodes'),
  );
  $themed_links = theme_item_list($variables);
  $num_of_pager_links = 5;
  return $themed_links . theme('pager', array('quantity' => $num_of_pager_links));
}

function show_matrix_visualization($nid) {
  $node = node_load($nid);
  return theme('agile_harris_matrix', array('nid' => $nid));
}

/**
 * Calls external python script.
 *
 * @param string $input_file
 *  harris matris xml file
 *
 * @return json
 *   jsaon data for d3 visualization
 */
function hmc2json($input_file) {
  $module_path = drupal_get_path('module', 'agile_harris_matrix');
  $path = DRUPAL_ROOT . '/' . "$module_path/python";
  $script = "$path/hmc2json.py";
  $executable = "/usr/bin/python";
  $output_file = drupal_tempnam('/tmp', 'hm');
  $ret = 0;
  $command = "{$executable} {$script} {$input_file} {$output_file}  2>&1";
  exec($command, $output, $ret);
  return file_get_contents($output_file);
}

/**
 * Callback to supply term to javascript.
 */
function agile_harris_matrix_get_term() {
  $vocabulary = $_POST['vocabulary'];
  $term = $_POST['term'];
  $terms = taxonomy_get_term_by_name($term, $vocabulary);
  $key = key($terms);
  $path = taxonomy_term_uri($terms[$key]);
  echo drupal_get_path_alias($path['path']);
}

<?php
/**
 * @file
 * The admin form for Agile Harris Matrix module.
 */

/**
 * Implementation of admin form. 
 *
 * @see drupal_get_form
 */
function agile_harris_matrix_admin_form($form, $form_state) {
  if (isset($form_state['triggering_element'])) {
    // Textfield AJAX callback.
    if ($form_state['triggering_element']['#name'] == 'agile_harris_matrix_path_textfield') {
      $python_path = $form_state['input']['agile_harris_matrix_path_textfield'];
    }
  }
  else {
    $python_path = variable_get('agile_harris_matrix_executable_path', '/usr/bin/python');
  }
  $conf_message = agile_harris_matrix_path_check($python_path);
  $form['agile_harris_matrix_wrapper'] = array(
    '#prefix' => '<div id="agile_harris_matrix_wrapper">',
    '#suffix' => '</div>',
  );
  $form['agile_harris_matrix_wrapper']['agile_harris_matrix_executable_path'] = array(
    '#type' => 'textfield',
    '#title' => t('System path to Python executable'),
    '#name' => 'agile_harris_matrix_path_textfield',
    '#description' => t('Enter the full path to Python executable.'),
    '#default_value' => $python_path,
    '#ajax' => array(
      'callback' => 'agile_harris_matrix_path_ajax',
      'wrapper' => 'agile_harris_matrix_wrapper',
    ),
  );
  $form['agile_harris_matrix_wrapper']['agile_harris_matrix_path_check'] = array(
    '#markup' => $conf_message,
  );

  $form['buttons']['submit'] = array('#type' => 'submit', '#value' => t('Save Configuration'));
  return $form;
}

/**
 * Checks path for the executable. 
 */
function agile_harris_matrix_path_check($python_path) {
  module_load_include('inc', 'islandora', 'includes/utilities');
  $output = array();
  $return_value = '';
  $command = 'stat ' . $python_path;
  exec($command, $output, $return_value);
  if ($return_value !== 0) {
    $confirmation_message = '<img src="' . url('misc/watchdog-error.png') . '"/>' . t('Unable to locate the Python executable at @pythonpath', array('@pythonpath' => $python_path));
  }
  else {
    $confirmation_message = '<img src="' . url('misc/watchdog-ok.png') . '"/>' . t('Python executable found at @pythonpath', array('@pythonpath' => $python_path));
  }
  return $confirmation_message;
}

/**
 * Ajax handler for admin form. 
 */
function agile_harris_matrix_path_ajax($form, $form_state) {
  return $form['agile_harris_matrix_wrapper'];
}
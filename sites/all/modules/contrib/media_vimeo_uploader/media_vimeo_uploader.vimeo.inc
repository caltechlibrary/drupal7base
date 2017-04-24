<?php
/**
 * @file
 * Vimeo related functions.
 */

/**
 * Returns instance of the Vimeo.php library.
 */
function media_vimeo_uploader_library_get() {
  // @todo add drush command to download video library.
  $library = libraries_load('vimeo.php');

  if (empty($library['loaded'])) {
    return FALSE;
  }

  $client_id = media_vimeo_uploader_client_id_get();
  $client_secret = media_vimeo_uploader_client_secret_get();
  $access_token = media_vimeo_uploader_access_token_get();
  $lib = new \Vimeo\Vimeo($client_id, $client_secret, $access_token);

  return $lib;
}


/**
 * Vimeo uploader form.
 */
function media_vimeo_uploader_upload_form($form, &$form_state, $step, $options) {
  switch ($step) {
    case 1:
      media_vimeo_uploader_file_form($form, $form_state, $options);
      break;

    case 2:
      media_vimeo_uploader_info_form($form, $form_state, $options);
      break;
  }

  return $form;
}

/**
 * Vimeo uploader file form.
 */
function media_vimeo_uploader_file_form(&$form, &$form_state, $options) {
  if (!empty($options['upload_link'])) {
    $form['#action'] = $options['upload_link'];
  }
  $form['file_data'] = array(
    '#name' => 'file_data',
    '#type' => 'file',
    '#title' => t('Choose a file'),
    '#description' => t('Select video file that will be uploaded to Vimeo.'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Next'),
  );
}

/**
 * Vimeo uploader info form.
 */
function media_vimeo_uploader_info_form(&$form, &$form_state, $options) {
  $form['title'] = array(
    '#type' => 'textfield',
    '#title' => 'Title',
    '#required' => TRUE,
    '#description' => t('Video title'),
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('Video description'),
  );
  $form['video_uri'] = array(
    '#type' => 'value',
    '#value' => !empty($options['video_uri']) ? $options['video_uri'] : '',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#id' => 'edit-next',
  );
  $form['#submit'][] = 'media_vimeo_uploader_info_form_submit';
}

/**
 * Vimeo uploader info form submit.
 */
function media_vimeo_uploader_info_form_submit(&$form, &$form_state) {
  $video_uri = $form_state['values']['video_uri'];
  list(,, $video_id) = explode('/', $video_uri);

  if (empty($video_id)) {
    form_set_error('video_uri', t('Vimeo uri %file is incorrect. An unknown error has occurred.', array('%file' => $video_uri)));
  }

  $options = array(
    'name' => $form_state['values']['title'],
    'description' => $form_state['values']['description'],
  );
  $lib = media_vimeo_uploader_library_get();
  $response = $lib->request('/videos/' . $video_id, $options, 'PATCH');

  // Check if everything ok and we have link to vimeo vide.
  if ($response['status'] !== 200 || empty($response['body']['link'])) {
    watchdog('media_vimeo_uploader', 'Vimeo update video error: @error', array('@error' => json_encode($response['body'])), WATCHDOG_ERROR);
    form_set_error('video_uri', t('Internal vimeo error. Please check !link.', array('!link' => l(t('log message'), 'admin/reports/dblog'))));
    return;
  }

  $video_url = $response['body']['link'];
  try {
    $provider = media_internet_get_provider($video_url);
    $file = $provider->save();
  }
  catch (Exception $e) {
    form_set_error('video_uri', $e->getMessage());
    return;
  }

  if (!$file->fid) {
    form_set_error('video_uri', t('The file with url %file could not be saved. An unknown error has occurred.', array('%file' => $video_url)));
    return;
  }
  else {
    // Save file via file_entity module.
    $form_state['storage']['upload'] = $file->fid;
    $form['#step'] = 1;
    module_load_include('inc', 'file_entity', 'file_entity.pages');
    file_entity_add_upload_submit($form, $form_state);
  }
}

/**
 * Returns Vimeo API Client Id.
 */
function media_vimeo_uploader_client_id_get() {
  return variable_get('media_vimeo_uploader_client_id', '');
}

/**
 * Returns Vimeo API Client Secret.
 */
function media_vimeo_uploader_client_secret_get() {
  return variable_get('media_vimeo_uploader_client_secret', '');
}

/**
 * Returns Vimeo API Access Token.
 */
function media_vimeo_uploader_access_token_get() {
  // @todo this may be dynamic variable.
  return variable_get('media_vimeo_uploader_access_token', '');
}

/**
 * Returns download URL for Vimeo video.
 */
function media_vimeo_uploader_vimeo_download_url($video_uri) {
  $url = '';
  $video_id = media_vimeo_uploader_extract_video_id($video_uri);
  $lib = media_vimeo_uploader_library_get();
  $response = $lib->request('/videos/' . $video_id, array(), 'GET');

  // Enable link to download file:
  // 1. Link is available(PRO account)
  // 2. Is public video.
  // 3. Video is already processed.
  if ($response['status'] == 200
    && !empty($response['body']['download'][0]['link'])
    && $response['body']['privacy']['download'] == TRUE
    && $response['body']['status'] == 'available') {
    $url = $response['body']['download'][0]['link'];
  }

  return $url;
}

/**
 * Returns video id.
 */
function media_vimeo_uploader_extract_video_id($uri) {
  $wrapper = file_stream_wrapper_get_instance_by_uri($uri);
  $parts = $wrapper->get_parameters();
  return !empty($parts['v']) ? $parts['v'] : '';
}

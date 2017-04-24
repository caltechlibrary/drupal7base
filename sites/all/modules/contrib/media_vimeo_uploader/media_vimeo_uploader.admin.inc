<?php
/**
 * @file
 * Admin pages and forms.
 */

/**
 * Vimeo API settings form.
 */
function media_vimeo_uploader_api_settings_form($form, &$form_state) {
  $form['vimeo_api'] = array(
    '#type' => 'fieldset',
    '#title' => t('Vimeo API Settings'),
    '#description' => t('To enable Vimeo API based access for vimeo, you must <a href="@url">register your application</a> with Vimeo and add the provided keys here.', array('@url' => 'https://developer.vimeo.com/apps')),
  );

  $form['vimeo_api']['media_vimeo_uploader_client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Client ID'),
    '#default_value' => media_vimeo_uploader_client_id_get(),
    '#description' => t('Client ID or API Key.'),
  );

  $form['vimeo_api']['media_vimeo_uploader_client_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('Client Secret'),
    '#default_value' => media_vimeo_uploader_client_secret_get(),
    '#description' => t('Client Secret or API Secret.'),
  );

  $form['vimeo_api']['media_vimeo_uploader_access_token'] = array(
    '#type' => 'textfield',
    '#title' => t('Access Token'),
    '#default_value' => media_vimeo_uploader_access_token_get(),
  );

  return system_settings_form($form);
}

<?php

/**
 * @file
 * Definition of VimeoUploaderBrowser.
 */

/**
 * Media browser plugin for Vimeo Video Uploader.
 */
class MediaVimeoUploaderBrowser extends MediaBrowserPlugin {
  /**
   * {@inheritdoc}
   */
  public function access($account = NULL) {
    return media_internet_access($account);
  }

  /**
   * {@inheritdoc}
   */
  public function view() {
    global $base_root;
    $step = 1;
    $build = array();
    $options = array();
    $params = $this->params;
    $params['vimeo_uploader'] = TRUE;

    // Display 2nd step with video information.
    $query = drupal_get_query_parameters();
    if (!empty($query['video_uri'])) {
      $options['video_uri'] = $query['video_uri'];
      $step = 2;
      $build['form'] = drupal_get_form('media_vimeo_uploader_upload_form', $step, $options);
      return $build;
    }

    // Get upload link for form action.
    $lib = media_vimeo_uploader_library_get();
    $options = array(
      'type' => 'POST',
      'redirect_url' => $base_root . request_uri(),
      'link' => '',
      // @todo need to be updated for PRO account.
      'upgrade_to_1080' => FALSE,
    );
    $response = $lib->request('/me/videos', $options, 'POST');

    if ($response['status'] !== 201 || empty($response['body']['upload_link_secure'])) {
      watchdog('plp_video_uploader', 'Vimeo upload form error: @error', array('@error' => json_encode($response['body'])), WATCHDOG_ERROR);
      $build['#markup'] = t('Internal vimeo error. Please check !link.', array('!link' => l(t('log message'), 'admin/reports/dblog')));
      return $build;
    }

    $options['upload_link'] = $response['body']['upload_link_secure'];
    $build['form'] = drupal_get_form('media_vimeo_uploader_upload_form', $step, $options);
    return $build;
  }
}

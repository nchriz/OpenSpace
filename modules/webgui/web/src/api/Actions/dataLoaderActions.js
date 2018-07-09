import { actionTypes } from './actionTypes';

export const setActivated = (isActivated) => ({
  type: actionTypes.setDataLoaderActivated,
  payload: {
    activated: isActivated
  }
});

export const setSelectedFilePaths = (selectedFilePaths) => ({
  type: actionTypes.setSelectedFilesPathName,
  payload: { selectedFilePaths }
});

export const setVolumesConvertedCount = (count) => ({
  type: actionTypes.setVolumesConvertedCount,
  payload: { currentVolumesConvertedCount: count }
});

export const setVolumesToConvertCount = (count) => ({
  type: actionTypes.setVolumesToConvertCount,
  payload: { currentVolumesToConvertCount: count }
});
